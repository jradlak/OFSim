#include <iostream>
#include <map>

#include "Window.h"

using namespace ofsim_renderer;
using namespace ofsim_events;

Window::Window(Camera& _camera, const u32 _width, const u32 _height)
	:camera(_camera), width(_width), height(_height)
{
	lastX = width / 2.0f;
	lastY = height / 2.0f;
	firstMouse = true;

	deltaTime = 0.0f;
	lastFrame = 0.0f;
}

int Window::initialize()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	mainWindow = glfwCreateWindow(width, height, "Symulator Lotow Orbitalnych", NULL, NULL);
	if (mainWindow == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(mainWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback);
	glfwSetCursorPosCallback(mainWindow, mouse_callback);
	glfwSetKeyCallback(mainWindow, key_callback);

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

void Window::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		EventProcessor::getInstance()->createUserEvent(UserAction::CHANGE_MODE_TO_FROM_PREDICTION, "");
	}

	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		EventProcessor::getInstance()->createUserEvent(UserAction::CHANGE_MODE_TO_FORM_PRESENTATION, "");
	}

	Camera_Movement direction = NONE;
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		direction = FORWARD;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		direction = BACKWARD;
	}

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		direction = LEFT;
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		direction = RIGHT;
	}

	if (direction != NONE) 
	{
		Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
		theWindow->camera.processKeyboard(direction, 0.4);
	}	
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}