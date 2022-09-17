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

#include "world/Model3D.h"

// GUI:
#include "gui\Gui.h"

// testing
#include "UnitTests.h"

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

Camera camera(glm::vec3(-100.0, -160.0, 1000.0));

int lastKeyPressed = 0;

typedef Model3D* ModelPtr;

unsigned __int64 currentTime();
int simulate(int argc, char** argv);
void switchGLStateForWorldRendering(float r, float g, float b);
void renderTelemetry(Gui* gui, Rocket& rocket, double altitude, double apogeum, double perygeum, double atmosphereDragForceMagnitude);
void syncFramerate(unsigned __int64 startTime, int ms_per_update);

void changeRocketRotationByKeyPressed(int keyPressed);

void makeModel(Model3D& cloud, Model3D& model, float angle, float dangle, glm::dvec3 rotation, CelestialBody& earth);

void reset(Rocket& rocket, PhysicsEngine* physics, VMachine* vm, glm::dvec3 rocketPos, glm::dvec3 towards);

bool runTests(int argc, char** argv);

std::string loadSourceCode(std::string sourcePath);

int main(int argc, char** argv)
{    
    while (simulate(argc, argv) != 0);
    return 0;
}

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

    glm::dvec3 rocketPos = earth.pointAboveTheSurface(angle, dangle, -0.2); //glm::dvec3(0.0, 0.0, 0.0); 
    Rocket rocket("model3d_shader", rocketPos, 0.000013);
    camera.position = rocket.getPosition() + glm::dvec3(0.0, 0.024, 0.0);
    rocket.init();

    //initialize Physics engine:
    int MS_PER_UPDATE = 12;
    glm::dvec3 towards = earth.pointAboveTheSurface(angle, dangle, -50.0);
    PhysicsEngine* physics = new PhysicsEngine(rocket, MS_PER_UPDATE);
    physics->changeAltitudeOrientation(CelestialBodyType::planet, 3185.0, towards);

    // initialize communication Bus:
    CommunicationBus* commandBus = new CommunicationBus();

    // initialize and start VM Thread:
    VMachine* vm = new VMachine(commandBus);
    VMTask* vmTask = new VMTask(vm);
    std::thread* vmThread = new std::thread(*vmTask);
    vmThread->detach();

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

    glm::dvec3 newRotation = glm::dvec3(-50.000021, 48.8000050, 0.0);
    glm::dvec3 deltaRotation = newRotation - rocket.getRotation();
    physics->rotateRocket(deltaRotation);

    // earth objects
    glm::dvec3 launchpadPos = earth.pointAboveTheSurface(angle, dangle, -0.187);
    Model3D launchpad("model3d_shader", "models/launchpad2.obj", launchpadPos, 0.05);
    launchpad.updateRotation(rocket.getRotation());
    const int numberOfObjects = 10;

    Model3D tree1("model3d_shader", "models/tree.obj", launchpadPos, 0.00007);
    Model3D cloud1("model3d_shader", "models/Cloud.obj", launchpadPos, 0.0017);
    makeModel(cloud1, tree1, angle, dangle, rocket.getRotation(), earth);

    Model3D tree2("model3d_shader", "models/tree.obj", launchpadPos, 0.00007);
    Model3D cloud2("model3d_shader", "models/Cloud.obj", launchpadPos, 0.0037);
    makeModel(cloud2, tree2, angle, dangle, rocket.getRotation(), earth);

    Model3D tree3("model3d_shader", "models/tree.obj", launchpadPos, 0.00007);
    Model3D cloud3("model3d_shader", "models/Cloud.obj", launchpadPos, 0.0027);
    makeModel(cloud3, tree3, angle, dangle, rocket.getRotation(), earth);

    Model3D tree4("model3d_shader", "models/tree.obj", launchpadPos, 0.00007);
    Model3D cloud4("model3d_shader", "models/Cloud.obj", launchpadPos, 0.0017);
    makeModel(cloud4, tree4, angle, dangle, rocket.getRotation(), earth);

    Model3D tree5("model3d_shader", "models/tree.obj", launchpadPos, 0.00007);
    Model3D cloud5("model3d_shader", "models/Cloud.obj", launchpadPos, 0.0037);
    makeModel(cloud5, tree5, angle, dangle, rocket.getRotation(), earth);

    Model3D tree6("model3d_shader", "models/tree.obj", launchpadPos, 0.00007);
    Model3D cloud6("model3d_shader", "models/Cloud.obj", launchpadPos, 0.0027);
    makeModel(cloud6, tree6, angle, dangle, rocket.getRotation(), earth);

    Model3D tree7("model3d_shader", "models/tree.obj", launchpadPos, 0.00007);
    Model3D cloud7("model3d_shader", "models/Cloud.obj", launchpadPos, 0.0007);
    makeModel(cloud7, tree7, angle, dangle, rocket.getRotation(), earth);

    Model3D tree8("model3d_shader", "models/tree.obj", launchpadPos, 0.00007);
    Model3D cloud8("model3d_shader", "models/Cloud.obj", launchpadPos, 0.0017);
    makeModel(cloud8, tree8, angle, dangle, rocket.getRotation(), earth);

    Model3D tree9("model3d_shader", "models/tree.obj", launchpadPos, 0.00007);
    Model3D cloud9("model3d_shader", "models/Cloud.obj", launchpadPos, 0.0037);
    makeModel(cloud9, tree9, angle, dangle, rocket.getRotation(), earth);

    Model3D tree10("model3d_shader", "models/tree.obj", launchpadPos, 0.00007);
    Model3D cloud10("model3d_shader", "models/Cloud.obj", launchpadPos, 0.0027);
    makeModel(cloud10, tree10, angle, dangle, rocket.getRotation(), earth);

    Model3D tree11("model3d_shader", "models/tree.obj", launchpadPos, 0.00007);
    Model3D cloud11("model3d_shader", "models/Cloud.obj", launchpadPos, 0.0017);
    makeModel(cloud11, tree11, angle, dangle, rocket.getRotation(), earth);

    Model3D tree12("model3d_shader", "models/tree.obj", launchpadPos, 0.00007);
    Model3D cloud12("model3d_shader", "models/Cloud.obj", launchpadPos, 0.0037);
    makeModel(cloud12, tree12, angle, dangle, rocket.getRotation(), earth);

    int simulationStopped = 0;
    double lastAltitude = 0;
    double apogeum = 0;
    double perygeum = 0;
    int lastAltitudeDirection = 1;
    int altitudeDirection = 1;
    while (!mainWindow.shouldClose() && !simulationStopped)
    {
        int factor = gui->getTimeFactor();
        // calculate lag:       

        if (factor == 0)
        {
            timePaused = currentTime() - previous;
            vm->setPause(true);
        }
        else if (factor > 0)
        {
            unsigned __int64 current = currentTime() - timePaused;
            unsigned __int64 elapsed = (current - previous) * factor;
            previous = current;
            lag += elapsed;
            runningTime += elapsed;
            vm->setPause(false);
            oddma->provideRunningTime(runningTime);
        }
        else if (factor == -1)
        {
            simulationStopped = 1;
            //gui->setTimeFactor(0);
            /*
            runningTime = 0;
            commandBus->clear();
            reset(rocket, physics, vm, earth.pointAboveTheSurface(angle, dangle, -0.2), earth.pointAboveTheSurface(angle, dangle, -50.0));
            gui->setTimeFactor(0);

            oddma->stop();

            delete commandBus;
            delete oddma;
            delete vmThread;
            delete vmTask;
            delete vm;

            commandBus = new CommunicationBus();
            VMachine* vm = new VMachine(commandBus);
            VMTask* vmTask = new VMTask(vm);
            std::thread* vmThread = new std::thread(*vmTask);
            vmThread->detach();

            oddma = new ODDMA(&rocket, physics, vm, commandBus);
            oddma->start();

            camera.position = rocket.getPosition() + glm::dvec3(0.0, 0.024, 0.0);
            */
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

        // render celestial bodies:
        earth.render(projection, view, lightPos);
        earthsMoon.render(projection, view, lightPos);
        sun.render(projection, view, lightPos);

        // render rocket:
        rocket.render(projection, view, lightPos);

        // render earth's objects:
        launchpad.render(projection, view, lightPos);

        tree1.render(projection, view, lightPos);
        tree2.render(projection, view, lightPos);

        tree3.render(projection, view, lightPos);
        tree4.render(projection, view, lightPos);
        tree5.render(projection, view, lightPos);
        tree6.render(projection, view, lightPos);
        tree7.render(projection, view, lightPos);
        tree8.render(projection, view, lightPos);
        tree9.render(projection, view, lightPos);
        tree10.render(projection, view, lightPos);
        tree11.render(projection, view, lightPos);
        tree12.render(projection, view, lightPos);

        cloud1.render(projection, view, lightPos);
        cloud2.render(projection, view, lightPos);
        cloud3.render(projection, view, lightPos);
        cloud4.render(projection, view, lightPos);
        cloud5.render(projection, view, lightPos);
        cloud6.render(projection, view, lightPos);
        cloud7.render(projection, view, lightPos);
        cloud8.render(projection, view, lightPos);
        cloud9.render(projection, view, lightPos);
        cloud10.render(projection, view, lightPos);
        cloud11.render(projection, view, lightPos);
        cloud12.render(projection, view, lightPos);

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
    vm->terminate();

    glfwTerminate();

    gui->cleanUp();

    delete physics;
    delete vm;
    delete gui;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    delete oddma;
    delete commandBus;

    delete vmThread;
    delete vmTask;

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

    void reset(Rocket& rocket, PhysicsEngine* physics, VMachine* vm, glm::dvec3 rocketPos, glm::dvec3 towards)
    {
        physics->changeAltitudeOrientation(CelestialBodyType::planet, 3185.0, towards);
        physics->restart();

        glm::dvec3 newRotation = glm::dvec3(-50.000021, 48.8000050, 0.0);
        glm::dvec3 deltaRotation = newRotation - rocket.getRotation();
        physics->rotateRocket(deltaRotation);

        rocket.reset(rocketPos);
        vm->reset();
    }

    void makeModel(Model3D& cloud, Model3D& model, float angle, float dangle, glm::dvec3 rotation, CelestialBody& earth)
    {
        float rangle = (rand() % 26 + 4) / 10000.0;
        float rdangle = (rand() % 26 + 4) / 10000.0;
        int min1 = rand() % 2;
        if (min1 == 0) rangle *= -1;
        int min2 = rand() % 2;
        if (min2 == 0) rdangle *= -1;

        glm::dvec3 treePos = earth.pointAboveTheSurface(angle + rangle, dangle + rdangle, -0.2);
        model.updateColor(0.2, 0.7, 0.1);
        model.updateRotation(rotation);
        model.updatePosition(treePos);

        rangle *= -20;
        rdangle *= -20;
        glm::dvec3 cloudPos = earth.pointAboveTheSurface(angle + rangle, dangle + rdangle, 0.01);
        cloud.updateColor(0.8, 0.8, 0.8);
        cloud.updateRotation(rotation);
        cloud.updatePosition(cloudPos);
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