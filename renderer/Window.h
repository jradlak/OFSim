#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Camera.h"

typedef void (*inputCall)(int);

class Window
{
public:
    Window(Camera& _camera, unsigned int _width, unsigned int _height);

    int initialize();
    void processInput();

    // function callback to precess key pressed
    void registerInputCallback(inputCall funct);

    bool shouldClose();
    void swapBuffers();

    GLFWwindow* getWindow();

    ~Window();

private:
    GLFWwindow* mainWindow;
    Camera& camera;
    unsigned int width, height;
    float lastX, lastY;
    bool firstMouse;

    
    float deltaTime; 	// time between current frame and last frame
    float lastFrame;

    inputCall processInputCall;

    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // ---------------------------------------------------------------------------------------------
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);


    // glfw: whenever the mouse moves, this callback is called
    // -------------------------------------------------------
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
       
};
