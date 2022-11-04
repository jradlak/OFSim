#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <map>
#include <string>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <thread>
#include <chrono> 

// world rendering
#include "renderer\Camera.h"
#include "renderer\Window.h"

// math and physics
#include "math_and_physics\PhysicsEngine.h"

// virtual machine:
#include "vmachine\VMachine.h"
#include "vmachine\ODDMA.h"
#include "vmachine\CommunicationBus.h"

#include "world/Launchpad.h"

#include "world/solar_system/CelestialBody.h"
#include "world/solar_system/SolarSystem.h"

// GUI:
#include "gui\Gui.h"

// testing
#include "UnitTests.h"

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

Camera camera(glm::vec3(-100.0, -160.0, 1000.0));

int lastKeyPressed = 0;

unsigned __int64 currentTime();
int simulate(int argc, char** argv);
void switchGLStateForWorldRendering(float r, float g, float b);
void renderTelemetry(Gui* gui, Rocket& rocket, double altitude, double apogeum, double perygeum, double atmosphereDragForceMagnitude);
void syncFramerate(unsigned __int64 startTime, int ms_per_update);

void changeRocketRotationByKeyPressed(int keyPressed);

bool runTests(int argc, char** argv);

std::string loadSourceCode(std::string sourcePath);

/*
int main(int argc, char** argv)
{    
    while (simulate(argc, argv) != 0);
    return 0;
}
*/

int simulate(int argc, char** argv)
{
    if (runTests(argc, argv))
    {
        return 0;
    }

    // main window preparation:
    Window mainWindow(camera, SCR_WIDTH, SCR_HEIGHT);
    int result = mainWindow.initialize();
    if (result != 0)
    {
        return result;
    }
    mainWindow.registerInputCallback(changeRocketRotationByKeyPressed);

    glm::dvec3 lightPos(0.0, -3185.0, 149600000.0);
    
    //glm::dvec3 lightPos(0.0, -3185.0, 149600000.0);   
    SolarSystem* solarSystem = new SolarSystem();
    
    //rocket and camera orientation:
    float angle = 30.0;
    float dangle = 60.0;

    glm::dvec3 rocketPos = solarSystem->pointAboveEarthSurface(angle, dangle, -0.2);
    Rocket rocket("model3d_shader", rocketPos, 0.000013);
    camera.position = rocket.getPosition() + glm::dvec3(0.0, 0.024, 0.0);
    rocket.init();

    //initialize Physics engine:
    int MS_PER_UPDATE = 12;
    glm::dvec3 towards = solarSystem->pointAboveEarthSurface(angle, dangle, -50.0);
    PhysicsEngine* physics = new PhysicsEngine(rocket, MS_PER_UPDATE);
    physics->changeAltitudeOrientation(CelestialBodyType::planet, 3185.0, towards);

    // initialize communication Bus:
    CommunicationBus* commandBus = new CommunicationBus();

    VMachine* vm = new VMachine(commandBus);
    vm->translateSourceCode("orbital_programs/ballisticProgram.oasm");
    vm->start();

    // initialize start ODDMA:
    ODDMA* oddma = new ODDMA(&rocket, physics, vm, commandBus);
    oddma->start();

    std::cout << "Start simulation! \n";

    unsigned __int64 lag = 0, previous = currentTime();
    glm::dmat4 projection = glm::perspective((double)glm::radians(camera.Zoom),
        (double)SCR_WIDTH / (double)SCR_HEIGHT, 0.001, 150000000.0);

    Gui* gui = new Gui();
    gui->initialization(&mainWindow);
    gui->loadButtonTextures();

    std::string sourceCode = loadSourceCode("orbital_programs/ballisticProgram.oasm");
    static char* srcStr = (char*)sourceCode.c_str();
    static char orbitalProgramSourceCode[1024 * 16];
    strcpy(orbitalProgramSourceCode, srcStr);

    unsigned __int64 startTime = currentTime();
    unsigned __int64 runningTime = 0;
    unsigned __int64 timePaused = 0;

    glm::dvec3 newRotation = glm::dvec3(-50.000021, 48.8000050, 0.0);
    glm::dvec3 deltaRotation = newRotation - rocket.getRotation();
    physics->rotateRocket(deltaRotation);

    solarSystem->provideRocketInformation(angle, dangle, &rocket);

    int simulationStopped = 0;
    double lastAltitude = 0;
    double apogeum = 0;
    double perygeum = 0;
    int lastAltitudeDirection = 1;
    int altitudeDirection = 1;

    // simulation loop:
    // -----------
    while (!mainWindow.shouldClose() && !simulationStopped)
    {
        int factor = gui->getTimeFactor();
        // calculate lag:       

        if (factor == 0)
        {
            timePaused = currentTime() - previous;
            vm->setPause();
        }
        else if (factor > 0)
        {
            unsigned __int64 current = currentTime() - timePaused;
            unsigned __int64 elapsed = (current - previous) * factor;
            previous = current;
            lag += elapsed;
            runningTime += elapsed;
            vm->unPause();
            oddma->provideRunningTime(runningTime);
        }
        else if (factor == -1)
        {
            simulationStopped = 1;          
        }

        // input
        mainWindow.processInput();

        // update physics:
        physics->updateKeyPressed(lastKeyPressed);
        lag = physics->calculateForces(lag);
        lastKeyPressed = 0;

        float* rgb = physics->atmosphereRgb();
        switchGLStateForWorldRendering(rgb[0], rgb[1], rgb[2]);

        gui->newFrame();

        // camera/view transformation:

        camera.updatePosition(rocket.getPosition(), rocket.getRotation());
        camera.processCameraRotation(3.0, 0);
        glm::dmat4 view = camera.getViewMatrix();

        // render whole solar system:       
        solarSystem->render(projection, view, lightPos);

        // render rocket:
        rocket.render(projection, view, lightPos);
             
        // render HUD:
        gui->renderSimulationControlWindow(runningTime);
        gui->renderCodeEditor(orbitalProgramSourceCode);
        std::map<unsigned __int64, RocketCommand>& commandHistory = commandBus->getCommandHistory();
        gui->renderCommandHistory(commandHistory);
        //gui->renderDiagnostics(rocket.getPosition(), rocket.getRotation());
        renderTelemetry(gui, rocket, physics->getAltitude(), apogeum, perygeum, physics->getAtmosphereDragForceMagnitude());
        
        if (lastAltitude < physics->getAltitude() && lastAltitude > 4)
        {
            altitudeDirection = 1;
        } 
        
        if (lastAltitude > physics->getAltitude() && lastAltitude > 4)
        {
            altitudeDirection = -1;
        }

        if (lastAltitudeDirection == 1 && altitudeDirection == -1) 
        {
            apogeum = lastAltitude;
        }

        if (lastAltitudeDirection == -1 && altitudeDirection == 1)
        {
            perygeum = lastAltitude;
        }

        lastAltitudeDirection = altitudeDirection;
        lastAltitude = physics->getAltitude();

        // sync and swap:
        syncFramerate(currentTime(), MS_PER_UPDATE);
        mainWindow.swapBuffers();
        glfwPollEvents();
    }

    // clean up:
    oddma->stop();
    vm->stop();

    glfwTerminate();

    gui->cleanUp();

    delete physics;
    delete vm;
    delete gui;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    delete oddma;
    delete commandBus;
   
    delete solarSystem;

    return simulationStopped;
}

    void switchGLStateForWorldRendering(float r, float g, float b)
    {
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(r, g, b, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void renderTelemetry(Gui* gui, Rocket& rocket, double altitude, double apogeum, double perygeum, double atmosphereDragForceMagnitude)
    {
        TelemetryData data;      

        data.altitude = altitude;
        data.mass = rocket.getMass();
        data.atmPressure = atmosphereDragForceMagnitude;
        glm::dvec3 velocity = rocket.getVelocity();
        data.velocity = glm::length(velocity);

        data.position = rocket.getPosition();
      
        data.apogeum = apogeum;               
        data.perygeum = perygeum;
        
        gui->renderTelemetry(data);
    }

    std::string loadSourceCode(std::string sourcePath)
    {
        std::ifstream sourceFile;

        std::string sourceCode = "";

        sourceFile.open(sourcePath.c_str(), std::ios::in);

        if (sourceFile.is_open()) {
            std::string line;

            while (sourceFile)
            {
                std::getline(sourceFile, line, '\r');
                sourceFile >> line;
                sourceCode += line + "\n";
            }

            sourceFile.close();
        }

        return sourceCode;
    }

    void syncFramerate(unsigned __int64 startTime, int ms_per_update)
    {
        unsigned __int64 endTime = startTime + ms_per_update;
        while (currentTime() < endTime)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    unsigned __int64 currentTime()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
            ).count();
    }

    bool runTests(int argc, char** argv)
    {
        if (argc > 1)
        {
            if (strcmp(argv[1], "-testingMode") == 0)
            {
                UnitTests* tests = new UnitTests();
                tests->run();
                delete tests;

                return true;
            }
        }

        return false;
    }

    void changeRocketRotationByKeyPressed(int keyPressed)
    {
        lastKeyPressed = keyPressed;
    }