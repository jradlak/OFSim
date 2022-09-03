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
#include "vmachine\VMTask.h";
#include "vmachine\ODDMA.h"
#include "vmachine\CommunicationBus.h"

#include "world/Launchpad.h"

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
int main(int argc, char** argv);
void switchGLStateForWorldRendering(float r, float g, float b);
void renderTelemetry(Gui* gui, Rocket& rocket, double altitude, double atmosphereDragForceMagnitude);
void syncFramerate(unsigned __int64 startTime, int ms_per_update);
void changeRocketRotationByKeyPressed(int keyPressed);
bool runTests(int argc, char** argv);

std::string loadSourceCode(std::string sourcePath);

int main(int argc, char** argv)
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

    // celestial bodies creation, podition and initialisation
    glm::dvec3 lightPos(0.0, -3185.0, 149600000.0);
    glm::dvec3 earthPos(0.0, -3186.0, 0.0);
    glm::dvec3 moonPos(384400.0, -3185.0, 0.0);

    CelestialBody sun(star, "light_source", 1392700, lightPos);
    CelestialBody earth(planet, "planet_shader", 6371.0, earthPos);
    CelestialBody earthsMoon(moon, "moon_shader", 1737.0, moonPos);

    sun.init();
    earth.init();
    earthsMoon.init();
   
    //rocket and camera orientation:
    float angle = 30.0;
    float dangle = 60.0;

    glm::dvec3 rocketPos = earth.pointAboveTheSurface(angle, dangle, 0.5); //glm::dvec3(0.0, 0.0, 0.0); 
    Rocket rocket("rocket_shader", rocketPos, 0.000013);
    camera.position = rocket.getPosition() + glm::dvec3(0.0, 0.024, 0.0);
    rocket.init();
 
    //initialize Physics engine:
    int MS_PER_UPDATE = 12;
    PhysicsEngine* physics = new PhysicsEngine(rocket, MS_PER_UPDATE);
    physics->changeAltitudeOrientation(CelestialBodyType::planet, 3185.0, earth.pointAboveTheSurface(angle, dangle, -10.0));

    // earth objects
    glm::dvec3 launchpadPos = earth.pointAboveTheSurface(angle, dangle, 0.52);
    Launchpad launchpad("rocket_shader", launchpadPos, 0.07);
    launchpad.updateRotation(rocket.getRotation());

    // initialize communication Bus:
    CommunicationBus* commandBus = new CommunicationBus();

    // initialize and start VM Thread:
    VMachine* vm = new VMachine(commandBus);
    VMTask vmTask(vm);
    std::thread vmThread(vmTask);
    vmThread.detach();

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

    // simulation loop:
    // -----------
    unsigned __int64 startTime = currentTime();
    unsigned __int64 runningTime = 0;

    unsigned __int64 timePaused = 0;    
    while (!mainWindow.shouldClose())
    {
        int factor = gui->getTimeFactor();
        // calculate lag:       
        if (factor == 0)
        {
            timePaused = currentTime() - previous;
        }
        else
        {
            unsigned __int64 current = currentTime() - timePaused;
            unsigned __int64 elapsed = (current - previous) * factor;
            previous = current;
            lag += elapsed;
            runningTime += elapsed;
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
        
        //camera.position = rocket.getPosition() + glm::dvec3(0.0, 0.024, 0.0);
        camera.updatePosition(rocket.getPosition(), rocket.getRotation());
        glm::dmat4 view = camera.getViewMatrix();

        // render celestial bodies:
        earth.render(projection, view, lightPos);
        earthsMoon.render(projection, view, lightPos);
        sun.render(projection, view, lightPos);

        // render rocket:
        rocket.render(projection, view, lightPos);
       
        // render earth's objects:
        launchpad.render(projection, view, lightPos);

        // render HUD:
        gui->renderSimulationControlWindow(runningTime);
        gui->renderCodeEditor(orbitalProgramSourceCode);
        std::map<unsigned __int64, RocketCommand>& commandHistory = commandBus->getCommandHistory();
        gui->renderCommandHistory(commandHistory);
        renderTelemetry(gui, rocket, physics->getAltitude(), physics->getAtmosphereDragForceMagnitude());

        // sync and swap:
        syncFramerate(currentTime(), MS_PER_UPDATE);
        mainWindow.swapBuffers();
        glfwPollEvents();
    }

    // clean up:
    oddma->stop();
    vm->terminate();

    glfwTerminate();

    gui->cleanUp();

    delete physics;
    delete vm;
    delete gui;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    delete oddma;
    delete commandBus;

    return 0;
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

void renderTelemetry(Gui* gui, Rocket& rocket, double altitude, double atmosphereDragForceMagnitude)
{
    TelemetryData data;
    
    data.altitude = altitude;
    data.mass = rocket.getMass();
    data.atmPressure = atmosphereDragForceMagnitude;
    glm::dvec3 velocity = rocket.getVelocity();
    data.velocity = glm::length(velocity);

    data.position = rocket.getPosition();
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

void changeRocketRotationByKeyPressed(int keyPressed)
{   
    lastKeyPressed = keyPressed;
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
