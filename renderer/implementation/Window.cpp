#include <iostream>
#include <map>

#include "..\Window.h"

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
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        mainWindow = glfwCreateWindow(width, height, "Symulator Lotow Orbitalnych", NULL, NULL);
        if (mainWindow == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }

        glfwMakeContextCurrent(mainWindow);

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

    GLFWwindow* Window::getWindow()
    {
        return mainWindow;
    }

    void Window::processInput()
    {        
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (glfwGetKey(mainWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(mainWindow, true);

        if (glfwGetKey(mainWindow, GLFW_KEY_M) == GLFW_PRESS)
        {
            processInputCall((int)GLFW_KEY_M);
        }

        if (glfwGetKey(mainWindow, GLFW_KEY_W) == GLFW_PRESS)
            camera.processKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(mainWindow, GLFW_KEY_S) == GLFW_PRESS)
            camera.processKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(mainWindow, GLFW_KEY_A) == GLFW_PRESS)
            camera.processKeyboard(LEFT, deltaTime);
        if (glfwGetKey(mainWindow, GLFW_KEY_D) == GLFW_PRESS)
            camera.processKeyboard(RIGHT, deltaTime);
        if (glfwGetKey(mainWindow, GLFW_KEY_Q) == GLFW_PRESS)
            camera.processKeyboard(ROLL_LEFT, deltaTime);
        if (glfwGetKey(mainWindow, GLFW_KEY_E) == GLFW_PRESS)
            camera.processKeyboard(ROLL_RIGHT, deltaTime);
    }

    void Window::registerInputCallback(inputCall funct)
    {
        processInputCall = funct;
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

        theWindow->camera.processMouseRotation(xoffset, yoffset);
    }

    Window::~Window()
    {
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
    }
