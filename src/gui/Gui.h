#pragma once

#include <string>
#include <map>
#include <vector>

#include "i18n.h"

#include "../renderer/Window.h"
#include "../simulation/TelemetryData.h"

#include "../infrastructure/RocketCommand.h"
#include "../python_integration/PythonError.h"

#include "../simulation/DiagnosticsData.h"
#include "../simulation/ManualControlData.h"

namespace ofsim_gui 
{
	// Class description in the header file
	class Gui
	{
	public:
		Gui() : i18n(I18n::getInstance()) {}
		~Gui() {}

		void initialization(Window* mainWindow);

		void newFrame();
		
		void renderMenuBar();

		void renderSplashScreen();

		void renderHelpScreen();

		void renderFileSaveAsDialog();

		void renderFileOpenDialog();

		void renderSimulationControlWindow(unsigned long long time);

		void renderCodeEditor(std::string& text);

        void renderTelemetry(ofsim_simulation::TelemetryData& telemetryData);
		
		void renderPresentationModeInfo(double distance);

        void renderTranslationErrors(ofsim_python_integration::PythonError &error);

		void restoreWindows();

		void endRendering();

		void plotTelemetry(
            std::vector<double> velocityHistory, double maxVelo,
			std::vector<double> altitudeHistory, double maxAlt,
			std::vector<double> atmPressureHistory, double maxAtm,
			std::vector<double> accelerationHistory, double maxAcc, double minAcc);

		void renderCommandHistory(std::map<u64, RocketCommand> &commandHistory);

        void renderDiagnostics(const ofsim_simulation::DiagnosticsData &diagnostics);

		void renderManualControlData(const ofsim_simulation::ManualControlData& manulaControl);

		void loadTextures();

		void cleanUp();

		int getTimeFactor();

		void setTimeFactor(int factor) { timeFactor = factor; }

		std::string getSavedFile() { return savedFile; }
		void clearSavedFile() { savedFile = ""; }
				
		bool getClearTranslationErrors() { return clearTranslationErrors; }
		void setClearTranslationErrors() { clearTranslationErrors = false; }

	private:
		GLuint play_texture { 0 };
		GLuint pause_texture { 0 };
		GLuint pp_texture { 0 };

		GLuint stop_texture { 0 };	
		GLuint fwd_texture { 0 };

		GLuint splashTexture { 0 };

		bool plaing { false };

		int out_width { 0 };
		int out_height { 0 };

		int timeFactor { 0 };

		bool viewSimulationControl { false };
		bool viewTelemetryOn { false };
		bool viewTelemetryPlot { false };
		bool viewCommands { false };
		bool viewSourceCode { false };
		bool viewFileOpen { false };
		bool viewFileSaveAs { false };
		bool viewSplashScreen { true };
		bool viewHelpScreen { false };

		bool clearTranslationErrors { false };

		std::vector<std::string> filesInDirectory;
		
		I18n* i18n;

		std::string directory { "/home/jakub/orb_progs" };
		std::string savedFile { "" };

		std::string helpText { "To be continued..." };

		int selcted_language_item { 0 };

		void loadFilesInDirectory(std::string &directory);	

		u64 currentTime();
	};
}
