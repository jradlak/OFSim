#include <iostream>
#include <map>

#include "Window.h"
#include "../infrastructure/EventProcessor.h"

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

    i18n = ofsim_gui::I18n::getInstance();
}

int Window::initialize()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    mainWindow = glfwCreateWindow(width, height, i18n->t(ofsim_gui::splash_title), NULL, NULL);
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
}

void Window::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		EventProcessor::getInstance()->createEvent(StateEvent::CHANGE_MODE_TO_FROM_PREDICTION, "");
	}

	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		EventProcessor::getInstance()->createEvent(StateEvent::CHANGE_MODE_TO_FORM_PRESENTATION, "");
	}

    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        EventProcessor::getInstance()->createEvent(StateEvent::CHANGE_MODE_TO_FROM_DIAGNOSTICS, "");
    }

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		EventProcessor::getInstance()->createEvent(StateEvent::CHANGE_MODE_TO_FROM_MANUAL_CONTROL, "");
	}

    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
    {
        EventProcessor::getInstance()->createEvent(StateEvent::ROTATION_LONGITUDE_UP, "");
    }

    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        EventProcessor::getInstance()->createEvent(StateEvent::ROTATION_LONGITUDE_DOWN, "");
    }

    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
    {
        EventProcessor::getInstance()->createEvent(StateEvent::ROTATION_LATITUDE_UP, "");
    }

    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
    {
        EventProcessor::getInstance()->createEvent(StateEvent::ROTATION_LATITUDE_DOWN, "");
    }

	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        EventProcessor::getInstance()->createEvent(StateEvent::THRUST_MAG_UP, "");
    }

	if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        EventProcessor::getInstance()->createEvent(StateEvent::THRUST_MAG_DOWN, "");
    }

	if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        EventProcessor::getInstance()->createEvent(StateEvent::ROTATE_X_UP, "");
    }

	if (key == GLFW_KEY_V && action == GLFW_PRESS)
    {
        EventProcessor::getInstance()->createEvent(StateEvent::ROTATE_X_DOWN, "");
    }

	if (key == GLFW_KEY_B && action == GLFW_PRESS)
    {
        EventProcessor::getInstance()->createEvent(StateEvent::ROTATE_Y_UP, "");
    }

	if (key == GLFW_KEY_N && action == GLFW_PRESS)
    {
        EventProcessor::getInstance()->createEvent(StateEvent::ROTATE_Y_DOWN, "");
    }

	if (key == GLFW_KEY_COMMA && action == GLFW_PRESS)
    {
        EventProcessor::getInstance()->createEvent(StateEvent::ROTATE_Z_UP, "");
    }

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        EventProcessor::getInstance()->createEvent(StateEvent::ROTATE_Z_DOWN, "");
    }


    f64 xoffset = 0, yoffset = 0;
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
        yoffset -= 10;
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
        yoffset += 10;
	}

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
        xoffset -= 10;
	}

	if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
        xoffset += 10;
	}

    if (xoffset != 0 || yoffset != 0)
	{
		Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));		        
        theWindow->camera.updateManualRotationPosition(xoffset, yoffset, 0.02);
    }

    if (key == GLFW_KEY_I && action == GLFW_PRESS)
    {
        EventProcessor::getInstance()->createEvent(StateEvent::SHOW_DIAGNOSTICS_IN_SIMULATION, "");
    }
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
