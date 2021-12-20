#include <iostream>
#include <map>
#include <string>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <thread>
#include <chrono> 

#include "Camera.h"
#include "Window.h"
#include "CelestialBody.h"
#include "Rocket.h"
#include "TextRenderer.h"

#include "Geometry.h"

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(-100.0, -160.0, 1000.0));

unsigned __int64 currentTime();
void switchGLStateForWorldRendering();
void renderTextHUD(TextRenderer* text, Rocket& rocket);
void syncFramerate(unsigned __int64 startTime, int ms_per_update);

int main()
{
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

    camera.Position = glm::dvec3(0.002);//earth.pointAboveTheSurface(34.5, 40.5, -0.05);

    sun.init();
    earth.init();
    earthsMoon.init();
    
    //TODO: coœ jest tu chyba nie tak z obliczaniem tego punktu
    float angle = 30.0;
    float dangle = 60.0;

    glm::dvec3 rocketPos = earth.pointAboveTheSurface(angle, dangle, 0.1); //earth.pointAboveTheSurface(30.0, 30.0, 0.05); //earth.pointAboveTheSurface(34.498, 40.5, -0.05);
    Rocket rocket("moon_shader", rocketPos, 1);

    camera.Position = rocket.getPosition() + glm::dvec3(0.0, 0.024, 0.0);
    rocket.init();
    
    // simulation loop
    // -----------
    unsigned __int64 lag = 0, previous = currentTime();
    int MS_PER_UPDATE = 12;
    glm::dvec3 rotation = glm::dvec3(0.0); // glm::dvec3(-20.0, 30.0, 0.0);
   
    while (!mainWindow.shouldClose())
    {
        unsigned __int64 current = currentTime();
        unsigned __int64 elapsed = current - previous;
        previous = current;
        lag += elapsed;

        // input
        // -----
        mainWindow.processInput();

        while (lag > MS_PER_UPDATE)
        {
            rocket.updatePhysics(MS_PER_UPDATE / 1000.0f);
            lag -= MS_PER_UPDATE;
        }

        switchGLStateForWorldRendering();

        // camera/view transformation
        camera.Position = rocket.getPosition() + glm::dvec3(0.0, 0.024, 0.0);
        glm::dmat4 view = camera.GetViewMatrix();

        earth.render(projection, view, lightPos);
        earthsMoon.render(projection, view, lightPos);
        sun.render(projection, view, lightPos);

        glm::vec3 direction = glm::normalize(rocket.getPosition() - earthPos);

        glm::quat qlook = lookAt(direction, glm::dvec3(0.0, 1.0, 0.0));
        rotation = glm::eulerAngles(qlook) * 180.0f / 3.14159f;
      
        rocket.updateRotation(rotation);

        rocket.render(projection, view, lightPos);

        renderTextHUD(text, rocket);

        syncFramerate(current, MS_PER_UPDATE);
        mainWindow.swapBuffers();
        glfwPollEvents();
    }

    glfwTerminate();
    delete text;

    return 0;
}

void switchGLStateForWorldRendering()
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderTextHUD(TextRenderer* text, Rocket& rocket)
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glm::dvec3 position = rocket.getPosition();
    std::stringstream ssPosition;
    ssPosition << "This is rocket position: (" << position.x << ", " << position.y << "," << position.y << ")";
    text->renderText(ssPosition.str(), 25.0f, 25.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f));

    glm::dvec3 velocity = rocket.getVelocity();
    std::stringstream ssVeloticy;
    ssVeloticy << "This is rocket velocity: (" << velocity.x << ", " << velocity.y << ",   " << velocity.y << ")";
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
