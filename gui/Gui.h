#pragma once

#include <string>
#include <map>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

#include "..\renderer\Window.h"

#include "..\engine\TelemetryData.h"
#include "..\vmachine\RocketCommand.h"

class Gui
{
public:
	Gui() {}
	~Gui() {}

	void initialization(Window* mainWindow);

	void newFrame();
	
	void renderSimulationControlWindow(unsigned __int64 time);

	void renderCodeEditor(std::string& text);

	void renderTelemetry(TelemetryData& telemetryData);

	void plotTelemetry(std::vector<double> velocityHistory, double maxVelo);

	void renderCommandHistory(std::map<unsigned __int64, RocketCommand> &commandHistory);

	void renderDiagnostics(glm::dvec3 position, glm::dvec3 rotation = glm::dvec3(0.0, 0.0, 0.0));

	void loadButtonTextures();

	void cleanUp();

	int getTimeFactor();

	void setTimeFactor(int factor) { timeFactor = factor; }

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