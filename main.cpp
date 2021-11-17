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

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(-100.0, -160.0, 1000.0));


unsigned __int64 currentTime();
void syncFramerate(unsigned __int64 startTime, int ms_per_update);

int main()
{
    Window mainWindow(camera, SCR_WIDTH, SCR_HEIGHT);
    int result = mainWindow.initialize();
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

    camera.Position = earth.pointAboveTheSurface(64.5, 40.5, -0.05);
    
    sun.init();
    earth.init();
    earthsMoon.init();

    glm::dvec3 rocketPos = earth.pointAboveTheSurface(64.5, 40.498, -0.05);
    Rocket rocket("moon_shader", rocketPos, 1);

    rocket.init();
    rocket.updateRotation(glm::dvec3(30.0, 0.0, -40.45));

    // draw lines only
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // render loop
    // -----------
    unsigned __int64 lag = 0, previous = currentTime();
    int MS_PER_UPDATE = 12;
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

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
        // camera/view transformation
        camera.Position = rocket.getPosition() + glm::dvec3(0.0, 0.024, 0.0);
        glm::dmat4 view = camera.GetViewMatrix();
        
        earth.render(projection, view, lightPos);
        earthsMoon.render(projection, view, lightPos);
        sun.render(projection, view, lightPos);

        rocket.render(projection, view, lightPos);

        syncFramerate(current, MS_PER_UPDATE);

        mainWindow.swapBuffers();
        glfwPollEvents();
    }
    
    return 0;
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