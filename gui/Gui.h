#pragma once

#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "..\renderer\Window.h"

#include <string>
#include "TelemetryData.h"

class Gui
{
public:
	Gui() {}
	~Gui() {}

	void initialization(Window* mainWindow);
	
	void newFrame();

	void renderSimulationControlWindow();

	void renderCodeEditor(static char text[]);

	void renderTelemetry(TelemetryData& telemetryData);
	
	void loadButtonTexture();

	void cleanUp();

private:
	GLuint out_texture = 0;
	int out_width = 0;
	int out_height = 0;
};