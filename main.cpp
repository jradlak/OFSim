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
#include "Camera.h"
#include "Window.h"
#include "TextRenderer.h"

// math and physics
#include "PhysicsEngine.h"

// testing
#include "UnitTests.h"

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(-100.0, -160.0, 1000.0));

int lastKeyPressed = 0;

unsigned __int64 currentTime();
void switchGLStateForWorldRendering(float r, float g, float b);
void renderTextHUD(TextRenderer* text, Rocket& rocket, double altitude);
void syncFramerate(unsigned __int64 startTime, int ms_per_update);
void changeRocketRotationByKeyPressed(int keyPressed);
bool runTests(int argc, char** argv);

int main(int argc, char** argv)
{
    if (runTests(argc, argv))
    {
        return 0;
    }

    Window mainWindow(camera, SCR_WIDTH, SCR_HEIGHT);
    int result = mainWindow.initialize();
    if (result != 0)
    {
        return result;
    }

    TextRenderer* text = new TextRenderer;
    result = text->init(SCR_WIDTH, SCR_HEIGHT);
    if (result != 0)
    {
        return result;
    }

    glm::dmat4 projection = glm::perspective((double)glm::radians(camera.Zoom),
        (double)SCR_WIDTH / (double)SCR_HEIGHT, 0.001, 150000000.0);

    // celestial bodies positions:
    glm::dvec3 lightPos(0.0, -3185.0, 149600000.0);
    glm::dvec3 earthPos(0.0, -3185.0, 0.0);
    glm::dvec3 moonPos(384400.0, -3185.0, 0.0);

    CelestialBody sun(star, "light_source", 1392700, lightPos);
    CelestialBody earth(planet, "planet_shader", 6371.0, earthPos);
    CelestialBody earthsMoon(moon, "moon_shader", 1737.0, moonPos);

    camera.Position = glm::dvec3(0.002);

    sun.init();
    earth.init();
    earthsMoon.init();
   
    float angle = 30.0;
    float dangle = 60.0;

    glm::dvec3 rocketPos = earth.pointAboveTheSurface(angle, dangle, 0.1);
    Rocket rocket("moon_shader", rocketPos, 1);

    camera.Position = rocket.getPosition() + glm::dvec3(0.0, 0.024, 0.0);
    rocket.init();
    
    unsigned __int64 lag = 0, previous = currentTime();
    int MS_PER_UPDATE = 12;
   
    mainWindow.registerInputCallback(changeRocketRotationByKeyPressed);

    //initialize Physic engine:
    PhysicsEngine* physics = new PhysicsEngine(rocket, MS_PER_UPDATE);
    physics->changeAltitudeOrientation(CelestialBodyType::planet, 3185.0, earth.pointAboveTheSurface(angle, dangle, -10.0));

    // simulation loop
    // -----------
    while (!mainWindow.shouldClose())
    {
        unsigned __int64 current = currentTime();
        unsigned __int64 elapsed = current - previous;
        previous = current;
        lag += elapsed;

        // input
        // -----
        mainWindow.processInput();

        // update physics:
        physics->updateKeyPressed(lastKeyPressed);
        lag = physics->calculateForces(lag);
        lastKeyPressed = 0;
        
        float* rgb = physics->atmosphereRgb();
        switchGLStateForWorldRendering(rgb[0], rgb[1], rgb[2]);

        // camera/view transformation:
        camera.Position = rocket.getPosition() + glm::dvec3(0.0, 0.024, 0.0);
        glm::dmat4 view = camera.GetViewMatrix();

        // render celestial bodies:
        earth.render(projection, view, lightPos);
        earthsMoon.render(projection, view, lightPos);
        sun.render(projection, view, lightPos);

        // render other objects
        rocket.render(projection, view, lightPos);

        renderTextHUD(text, rocket, physics->getAltitude());

        syncFramerate(current, MS_PER_UPDATE);
        mainWindow.swapBuffers();
        glfwPollEvents();
    }

    glfwTerminate();
    
    delete text;
    delete physics;

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

void renderTextHUD(TextRenderer* text, Rocket& rocket, double altitude)
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    std::stringstream ssAltitude;
    ssAltitude << "Wysokosc rakiety: " << altitude << " km";
    text->renderText(ssAltitude.str(), 25.0f, 75.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));

    glm::dvec3 position = rocket.getPosition();
    std::stringstream ssPosition;
    ssPosition << "Pozycja rakiety: (" << position.x << ", " << position.y << "," << position.y << ")";
    text->renderText(ssPosition.str(), 25.0f, 25.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));

    glm::dvec3 velocity = rocket.getVelocity();
    std::stringstream ssVeloticy;
    ssVeloticy << "Wektor predkosci rakiety: (" << velocity.x << ", " << velocity.y << ",   " << velocity.y << ")";
    text->renderText(ssVeloticy.str(), 25.0f, 50.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));
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
