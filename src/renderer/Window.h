#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../gui/i18n.h"
#include "../math_and_physics/MathTypes.h"

#include "Camera.h"

using namespace ofsim_renderer;

class Window
{
public:
    Window(Camera& _camera, const u32 _width, const u32 _height);

    int initialize();    
    
    bool shouldClose() { return glfwWindowShouldClose(mainWindow); }
    void swapBuffers() { glfwSwapBuffers(mainWindow); }

    GLFWwindow* getWindow() { return mainWindow; }

    ~Window();

private:
    GLFWwindow* mainWindow = nullptr;
    Camera& camera;

    const u32 width, height;
    float lastX, lastY;
    bool firstMouse;

    float deltaTime;
    float lastFrame;

    int eventsCreated = 0;

    ofsim_gui::I18n* i18n;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);       

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
