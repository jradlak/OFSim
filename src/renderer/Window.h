#pragma once

#include "../../external_libraries/glad/glad.h"
#include <GLFW/glfw3.h>

#include "Camera.h"

typedef void (*inputCall)(int);

using namespace ofsim_renderer;

class Window
{
public:
    Window(Camera& _camera, unsigned int _width, unsigned int _height);

    int initialize();
    void processInput();
    
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

    float deltaTime;
    float lastFrame;

    inputCall processInputCall;
    
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);       
};
