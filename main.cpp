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

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(-100.0, -160.0, 1000.0));

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

    camera.Position = earth.pointAboveTheSurface(64.5, 40.5, 0.1);
    
    sun.init();
    earth.init();
    earthsMoon.init();

    // draw lines only
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // render loop
    // -----------
    while (!mainWindow.shouldClose())
    {
        // input
        // -----
        mainWindow.processInput();

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
        // camera/view transformation
        glm::dmat4 view = camera.GetViewMatrix();
        
        earth.render(projection, view, lightPos);
        earthsMoon.render(projection, view, lightPos);
        sun.render(projection, view, lightPos);

        mainWindow.swapBuffers();
        glfwPollEvents();

        std::this_thread::sleep_for(std::chrono::milliseconds(12));
    }
    
    return 0;
}
