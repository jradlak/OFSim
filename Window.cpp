#include <iostream>
#include "Window.h"

Window::Window(Camera& _camera, unsigned int _width, unsigned int _height)
        :camera(_camera)
    {
        width = _width;
        height = _height;

        lastX = width / 2.0f;
        lastY = height / 2.0f;
        firstMouse = true;

        deltaTime = 0.0f;
        lastFrame = 0.0f;
    }

    int Window::initialize()
    {
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // glfw window creation
        // --------------------
        mainWindow = glfwCreateWindow(width, height, "Orbital Flight Simulator", NULL, NULL);
        if (mainWindow == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }

        glfwMakeContextCurrent(mainWindow);

        //Allow modern extension features
        glewExperimental = GL_TRUE;

        if (glewInit() != GLEW_OK)
        {
            printf("GLEW initialization failed!");
            glfwDestroyWindow(mainWindow);
            glfwTerminate();

            return 1;
        }

        glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback);
        glfwSetCursorPosCallback(mainWindow, mouse_callback);
        glfwSetScrollCallback(mainWindow, scroll_callback);

        // tell GLFW to capture our mouse
        glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);

        glfwSetWindowUserPointer(mainWindow, this);

        return 0;
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(mainWindow);
}

void Window::swapBuffers()
{
    glfwSwapBuffers(mainWindow);
}

 // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void Window::processInput()
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (glfwGetKey(mainWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(mainWindow, true);

        if (glfwGetKey(mainWindow, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(mainWindow, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(mainWindow, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(mainWindow, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
        if (glfwGetKey(mainWindow, GLFW_KEY_Q) == GLFW_PRESS)
            camera.ProcessKeyboard(ROLL_LEFT, deltaTime);
        if (glfwGetKey(mainWindow, GLFW_KEY_E) == GLFW_PRESS)
            camera.ProcessKeyboard(ROLL_RIGHT, deltaTime);

    }

    void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void Window::mouse_callback(GLFWwindow* window, double xpos, double ypos)
    {
        Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

        if (theWindow->firstMouse)
        {
            theWindow->lastX = xpos;
            theWindow->lastY = ypos;
            theWindow->firstMouse = false;
        }

        float xoffset = xpos - theWindow->lastX;
        float yoffset = theWindow->lastY - ypos; // reversed since y-coordinates go from bottom to top

        theWindow->lastX = xpos;
        theWindow->lastY = ypos;

        theWindow->camera.ProcessMouseMovement(xoffset, yoffset);
    }

    void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        theWindow->camera.ProcessMouseScroll(yoffset);
    }

    Window::~Window()
    {
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
    }
