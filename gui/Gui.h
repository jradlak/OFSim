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

	void renderSimulationControlWindow(unsigned __int64 time);

	void renderCodeEditor(static char text[]);

	void renderTelemetry(TelemetryData& telemetryData);

	void loadButtonTextures();

	void cleanUp();

	int getTimeFactor() { return timeFactor; }

private:
	GLuint play_texture = 0;
	GLuint pause_texture = 0;
	GLuint pp_texture = 0;

	GLuint stop_texture = 0;	
	GLuint fwd_texture = 0;

	bool plaing = false;

	int out_width = 0;
	int out_height = 0;

	int timeFactor = 0;
};