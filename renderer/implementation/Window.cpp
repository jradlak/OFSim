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
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // glfw window creation
        // --------------------
        mainWindow = glfwCreateWindow(width, height, "Symulator Lotow Orbitalnych", NULL, NULL);
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
        //glfwSetCursorPosCallback(mainWindow, mouse_callback);        

        // tell GLFW to capture our mouse
        //glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


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

    GLFWwindow* Window::getWindow()
    {
        return mainWindow;
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

        /*
        if (glfwGetKey(mainWindow, GLFW_KEY_UP) == GLFW_PRESS) 
        {
            processInputCall((int)GLFW_KEY_UP);
        }

        if (glfwGetKey(mainWindow, GLFW_KEY_DOWN) == GLFW_PRESS) 
        {
            processInputCall((int)GLFW_KEY_DOWN);
        }

        if (glfwGetKey(mainWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            processInputCall((int)GLFW_KEY_LEFT);
        }

        if (glfwGetKey(mainWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            processInputCall((int)GLFW_KEY_RIGHT);
        }
        */

        cameraRotationHandler(mainWindow);
    }

    void Window::registerInputCallback(inputCall funct)
    {
        processInputCall = funct;
    }

    void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void Window::cameraRotationHandler(GLFWwindow* window)
    {
        double xoffset = 0;
        double yoffset = 0;
        
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
            xoffset = 10;
            theWindow->camera.processCameraRotation(xoffset, yoffset);
        }
        
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
            xoffset = -10;
            theWindow->camera.processCameraRotation(xoffset, yoffset);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
            yoffset = 10;
            theWindow->camera.processCameraRotation(xoffset, yoffset);
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
            yoffset = -10;
            theWindow->camera.processCameraRotation(xoffset, yoffset);
        }
    }

    Window::~Window()
    {
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
    }
