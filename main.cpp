#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Window.h"
#include "CelestialBody.h"

#include <iostream>
#include <vector>

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 10.0f, 0.0f));

int main()
{
    Window mainWindow(camera, SCR_WIDTH, SCR_HEIGHT);
    int result = mainWindow.initialize();
    if (result != 0)
    {
        return result;
    }

    // celestial bodies positions:
    glm::vec3 lightPos(0.0f, 0.0f, 86125.5f);
    glm::vec3 earthPos(0.0f, 0.0f, 0.0f);
    glm::vec3 moonPos(220.0f, 2.5f, 0.0f);

    CelestialBody sun(star, "light_source", 801.78f, lightPos);
    CelestialBody earth(planet, "shader", 3.67f, earthPos);
    CelestialBody earthsMoon(moon, "shader", 1.0f, moonPos);

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

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 90000.0f);
        
        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
       
        earth.render(projection, view, lightPos);
        earthsMoon.render(projection, view, lightPos);

        sun.render(projection, view, lightPos);
       
        mainWindow.swapBuffers();
        glfwPollEvents();
    }

    return 0;
}
