#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "..\renderer\Window.h"

class Gui
{
public:
	Gui() {}
	~Gui() {}

	void initialization(Window* mainWindow);
	
	void newFrame();
	void render();
	
	void cleanUp();
};