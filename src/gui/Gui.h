#pragma once

#include <string>
#include <map>

#include <filesystem>
namespace fs = std::filesystem;

#include "../../external_libraries/imgui/imgui.h"
#include "../../external_libraries/imgui/imgui_impl_glfw.h"
#include "../../external_libraries/imgui/imgui_impl_opengl3.h"
#include "../../external_libraries/imgui/imgui_stdlib.h"

#include "../renderer/Window.h"

#include "../simulation/TelemetryData.h"
#include "../vmachine/RocketCommand.h"
#include "i18n.h"

#include "../simulation/FileService.h"

enum MenuPosition {
	FILE_NEW,
	FILE_OPEN,
	FILE_SAVE,	
	FILE_SAVED_AS,
	FILE_EXIT,

	VIEW_TELEMETRY,
	VIEW_TELEMETRY_PLOT,
	VIEW_COMMANDS,
	VIEW_PROGRAM,

	HELP_ABOUT,
	HELP_HELP,

	NONE
};

class Gui
{
public:
	Gui() { i18n = I18n::getInstance(); }
	~Gui() {}

	void initialization(Window* mainWindow);

	void newFrame();
	
	void renderMenuBar();

	void renderSplashScreen();

	void renderFileSaveAsDialog();

	void renderFileOpenDialog();

	void renderSimulationControlWindow(unsigned long long time);

	void renderCodeEditor(std::string& text);

	void renderTelemetry(TelemetryData& telemetryData);
	
	void renderPresentationModeInfo(double distance);

	void renderTranslationErrors(std::string errors);

	void restoreWindows();

	void endRendering();

	void plotTelemetry(
		std::vector<double> velocityHistory, double maxVelo,
		std::vector<double> altitudeHistory, double maxAlt,
		std::vector<double> atmPressureHistory, double maxAtm,
		std::vector<double> accelerationHistory, double maxAcc, double minAcc);

	void renderCommandHistory(std::map<unsigned long long, RocketCommand> &commandHistory);

	void renderDiagnostics(glm::dvec3 position, glm::dvec3 rotation = glm::dvec3(0.0, 0.0, 0.0));

	void loadTextures();

	void cleanUp();

	int getTimeFactor();

	void setTimeFactor(int factor) { timeFactor = factor; }

	MenuPosition getLastClickedMenu() { return lastClickedMenu; }
	void clearLastClickedMenu() { lastClickedMenu = MenuPosition::NONE; }

	std::string getSavedFile() { return savedFile; }
	void clearSavedFile() { savedFile = ""; }
	std::string getSelectedFile() { return selectedFile; }
	void clearSelectedFile() { selectedFile = ""; }

	bool getClearTranslationErrors() { return clearTranslationErrors; }
	void setClearTranslationErrors() { clearTranslationErrors = false; }

private:
	GLuint play_texture = 0;
	GLuint pause_texture = 0;
	GLuint pp_texture = 0;

	GLuint stop_texture = 0;	
	GLuint fwd_texture = 0;

	GLuint splashTexture = 0;

	bool plaing = false;

	int out_width = 0;
	int out_height = 0;

	int timeFactor = 0;

	bool viewSimulationControl = false;
	bool viewTelemetryOn = false;
	bool viewTelemetryPlot = false;
	bool viewCommands = false;
	bool viewSourceCode = false;
	bool viewFileOpen = false;
	bool viewFileSaveAs = false;
	bool viewSplashScreen = true;

	bool clearTranslationErrors = false;

	std::vector<std::string> filesInDirectory;

	MenuPosition lastClickedMenu = MenuPosition::NONE;
	
	I18n* i18n;

	std::string directory;
	std::string selectedFile = "";
	std::string savedFile = "";

	int selcted_language_item = 0;

	void loadFilesInDirectory(std::string &directory);	
};