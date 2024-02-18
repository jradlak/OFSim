#pragma once

#include "../../external_libraries/glad/glad.h"
#include "../math_and_physics/MathTypes.h"

#include <GLFW/glfw3.h>

#include "Camera.h"

using namespace ofsim_renderer;

class Window
{
public:
    Window(Camera& _camera, const u32 _width, const u32 _height);

    int initialize();
    void processInput();
    
    bool shouldClose();
    void swapBuffers();

    GLFWwindow* getWindow();

    ~Window();

private:
    GLFWwindow* mainWindow = nullptr;
    Camera& camera;

    const u32 width, height;
    float lastX, lastY;
    bool firstMouse;

    float deltaTime;
    float lastFrame;
    
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);       
};
