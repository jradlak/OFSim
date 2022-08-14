#pragma once

#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include "..\renderer\Window.h"

#include "TelemetryData.h"
#include <string>

class Gui
{
public:
	Gui() {}
	~Gui() {}

	void initialization(Window* mainWindow);
	
	void newFrame();

	void renderCodeEditor(static char text[]);

	void renderTelemetry(TelemetryData& telemetryData);
	
	void cleanUp();
};