#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "..\renderer\Window.h"

#include "TelemetryData.h"

class Gui
{
public:
	Gui() {}
	~Gui() {}

	void initialization(Window* mainWindow);
	
	void newFrame();
	void render();

	void renderTelemetry(TelemetryData& telemetryData);
	
	void cleanUp();
};