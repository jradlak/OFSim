#include "Gui.h"
#include "../infrastructure/ImageLoader.h"

#include <iostream>

#include <string>

#include "../../external_libraries/imgui/imgui.h"
#include "../../external_libraries/imgui/imgui_impl_glfw.h"
#include "../../external_libraries/imgui/imgui_impl_opengl3.h"
#include "../../external_libraries/imgui/imgui_stdlib.h"

#include "../infrastructure/EventProcessor.h"
#include "../infrastructure/FileService.h"

using namespace ofsim_gui;
using namespace ofsim_events;
using namespace ofsim_simulation;

void Gui::initialization(Window* mainWindow)
{
    // ImGui initialization:
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(mainWindow->getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    std::string manualPath = i18n->currentLanguage == Language::PL 
        ? "doc/operation_manual_pl.txt"
        : "doc/operation_manual_en.txt";

    helpText = ofsim_infrastructure::loadTextFile(manualPath);    
}

void Gui::newFrame()
{     
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::renderMenuBar()
{
    EventProcessor* eventProcessor = EventProcessor::getInstance();
    
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Program"))
        {
            if (ImGui::MenuItem(i18n->t(menu_new))) 
            { 
                eventProcessor->createEvent(StateEvent::FILE_NEW, ""); 
            }

            if (ImGui::MenuItem(i18n->t(menu_open))) 
            {                 
                viewFileOpen = true;
                eventProcessor->createEvent(StateEvent::SHOW_FILE_OPEN_DIALOG, "");                
            }

            if (ImGui::MenuItem(i18n->t(menu_save))) 
            { 
                eventProcessor->createEvent(StateEvent::FILE_SAVE, "");
            }

            if (ImGui::MenuItem(i18n->t(menu_save_as)))
            {
                viewFileSaveAs = true;                
            }
            
            ImGui::Separator();
            if (ImGui::MenuItem(i18n->t(menu_close))) 
            {  
                eventProcessor->createEvent(StateEvent::FILE_EXIT, "");
            }

            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu(i18n->t(menu_view)))
        {            
            ImGui::Checkbox(i18n->t(menu_telemetry_data), &viewTelemetryOn);
            ImGui::Checkbox(i18n->t(menu_telemetry_plots), &viewTelemetryPlot);
            ImGui::Checkbox(i18n->t(menu_command_history), &viewCommands);
            ImGui::Separator();
            ImGui::Checkbox(i18n->t(menu_program_source_code), &viewSourceCode);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(i18n->t(menu_help)))
        {
            if (ImGui::MenuItem(i18n->t(menu_manual))) 
            { 
                eventProcessor->createEvent(StateEvent::HELP_HELP, "");
                viewHelpScreen = true;          
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem(i18n->t(menu_about))) 
            { 
                eventProcessor->createEvent(StateEvent::HELP_ABOUT, "");
                viewSplashScreen = true;
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }    
}

void Gui::renderSplashScreen()
{
    if (!viewSplashScreen) return;

    ImGui::SetNextWindowSize(ImVec2(772, 590), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(580, 200), ImGuiCond_Once);

    ImGui::Begin(i18n->t(splash_title));

    ImGui::Image((void*)(intptr_t)splashTexture, ImVec2(767, 352));

    ImGui::NewLine();

    ImGui::TextWrapped("%s", i18n->t(splash_info));

    ImGui::NewLine();
   
    ImGui::Separator();

    ImGui::Text("%s", i18n->t(splash_version));
    ImGui::Text("Jakub Radlak 2021 - 2024");

    ImGui::Separator();
    ImGui::NewLine();
    
    if (ImGui::Button("OK", ImVec2(120, 0)))
    {        
        viewSplashScreen = false;

        viewTelemetryOn = true;
        viewTelemetryPlot = true;
        viewCommands = true;
        viewSourceCode = true;
        viewSimulationControl = true;
    }

    ImGui::SameLine();
    static int item_current = 0;
    ImGui::Combo("language/jezyk", &item_current, "PL\0EN");
    if (item_current != selcted_language_item)
    {
        selcted_language_item = item_current;
        if (item_current == 0) i18n->switchLanguage(PL);
        if (item_current == 1) i18n->switchLanguage(EN);
    }

    ImGui::End();
}

void ofsim_gui::Gui::renderHelpScreen()
{
    if (!viewHelpScreen) return;

    ImGui::SetNextWindowSize(ImVec2(772, 890), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(580, 50), ImGuiCond_Once);
    
    ImGui::Begin(i18n->t(help_title));

    ImGui::Separator();
    ImGui::NewLine();
    
    ImGui::TextWrapped("%s", helpText.c_str());

    if (ImGui::Button("OK", ImVec2(120, 0)))
    {        
        viewHelpScreen = false;
    }

    ImGui::End();
}

void Gui::renderFileOpenDialog()
{   
    if (!viewFileOpen) return;

    fileOpenDialog.renderFileDialog();    

    viewFileOpen = fileOpenDialog.viewFileOpen;
}

void Gui::renderFileSaveAsDialog()
{
    if (!viewFileSaveAs) return;

    fileSaveDialog.renderFileDialog();

    viewFileSaveAs = fileSaveDialog.viewFileOpen;    
}

void Gui::renderSimulationControlWindow(unsigned long long time)
{
    if (!viewSimulationControl) return;

    EventProcessor* eventProcessor = EventProcessor::getInstance();

    ImGui::SetNextWindowSize(ImVec2(450, 100), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(60, 30), ImGuiCond_Once);

    ImGui::Begin(i18n->t(sim_panel_title));

    unsigned long long seconds = time / 1000;
    unsigned long long millis = time % 1000;

    std::string strClock = i18n->t(sim_panel_clock) + std::to_string(seconds) + "." + std::to_string(millis) + "s";
    ImGui::Text("%s", strClock.c_str());

    ImGui::SameLine();
    std::string strTimeFactor = i18n->t(sim_panel_time_compression) + std::to_string(timeFactor) + "x";
    ImGui::Text("%s", strTimeFactor.c_str());

    if (ImGui::ImageButton((void*)(intptr_t)pp_texture, ImVec2(32, 32))) 
    {
        if (playing)
        {
            pp_texture = play_texture;
            playing = false;
            timeFactor = 0;
        }
        else
        {
            pp_texture = pause_texture;
            playing = true;
            timeFactor = 1;

            eventProcessor->createEvent(StateEvent::START_SIMULATION, "");
        }
    }
    
    ImGui::SameLine();

    if (ImGui::ImageButton((void*)(intptr_t)stop_texture, ImVec2(32, 32))) {
        pp_texture = play_texture;
        playing = false;
        timeFactor = -1;
        eventProcessor->createEvent(StateEvent::PYTHON_PROGRAM_EXECUTION_STOP, "");
    }

    ImGui::SameLine();

    if (ImGui::ImageButton((void*)(intptr_t)fwd_texture, ImVec2(32, 32))) {
        std::cout << "Kliknieto fwd!! \n";

        if (timeFactor == 16)
        {
            timeFactor = 1;
        } 
        else if (timeFactor > 0)
        {
            timeFactor *= 2;
        }
    }

    ImGui::SameLine();
    if (ImGui::Checkbox(i18n->t(camera_autorotation), &cameraAutorotation)) 
    {
        eventProcessor->createEvent(StateEvent::CAMERA_AUTOROTATION, 
         cameraAutorotation ? "1" : "0");
    }   

    ImGui::End();
}

void Gui::renderCodeEditor(std::string& text)
{
    if (!viewSourceCode) return;

    ImGui::SetNextWindowSize(ImVec2(450, 780), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(60, 140), ImGuiCond_Once);
    
    ImGui::Begin(i18n->t(code_editor_title));

    int numLines = 1;
    for (int i = 0; text[i]; i++) {
        if (text[i] == '\n') {
            numLines++;
        }
    }

    ImGui::BeginChild("TextEditor", ImVec2(450, ImGui::GetTextLineHeight() * 257), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    // Draw the line numbers
    ImGui::BeginChild("LineNumbers", ImVec2(30, 0), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    for (int i = 1; i <= numLines; i++) {
        ImGui::Text("%d.", i);
    }

    ImGui::EndChild();

    ImGui::SameLine();
    
    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;   
    ImGui::InputTextMultiline("##source", &text, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 257), flags);
    currentlyEdit = ImGui::IsItemFocused();
    
    ImGui::EndChild();

    ImGui::End();
}

void Gui::renderTelemetry(TelemetryData& telemetryData)
{
    if (!viewTelemetryOn) return;

    ImGui::SetNextWindowSize(ImVec2(450, 150), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(1290, 30), ImGuiCond_Once);

    ImGui::Begin(i18n->t(telemetry_title));
    
    std::string ssAltitude = i18n->t(telemetry_altitude) + std::to_string(telemetryData.altitude) + " km";
    ImGui::Text("%s", ssAltitude.c_str());

    std::string ssMass = i18n->t(telemetry_mass) + std::to_string(telemetryData.mass) + "t";
    ImGui::Text("%s", ssMass.c_str());

    std::string ssPressure = i18n->t(telemetry_atm_press) + std::to_string(telemetryData.atmPressure * 10);
    ImGui::Text("%s", ssPressure.c_str());

    std::string ssVelocity = i18n->t(telemetry_velocity) + std::to_string(telemetryData.velocity) + "km/s";
    ImGui::Text("%s", ssVelocity.c_str());

    std::string ssPosition = i18n->t(telemetry_position) +
        std::to_string(telemetryData.position.x) + ", " +
        std::to_string(telemetryData.position.y) + ", " +
        std::to_string(telemetryData.position.z) + ")";
    ImGui::Text("%s", ssPosition.c_str());

    std::string ssApogeum = i18n->t(telemetry_apogee);
    if (telemetryData.apogee != 0)
    {
        ssApogeum = i18n->t(telemetry_apogee) + std::to_string(telemetryData.apogee) + " km";
    }

    std::string ssPerygeum = i18n->t(telemetry_perigee);
    if (telemetryData.perigee != 0)
    {
        ssPerygeum = i18n->t(telemetry_perigee) + std::to_string(telemetryData.perigee) + " km";
    }

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", ssApogeum.c_str());
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "%s", ssPerygeum.c_str());
    
    ImGui::End();
}

void Gui::plotTelemetry(
    std::vector<double> velocityHistory, double maxVelo,
    std::vector<double> altitudeHistory, double maxAlt,
    std::vector<double> atmPressureHistory, double maxAtm,
    std::vector<double> accelerationHistory, double maxAcc, double minAcc)
{
    if (!viewTelemetryPlot) return;

    ImGui::SetNextWindowSize(ImVec2(450, 510), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(1290, 190), ImGuiCond_Once);
    ImGui::Begin(i18n->t(telemetry_plots_title));

    int n = velocityHistory.size();
    float* arrVelo = new float[n];
    float* arrAlt = new float[n];
    float* arrAcc = new float[n];
    float* arrAtm = new float[n];

    for (int i = 0; i < n; i++)
    {
        arrVelo[i] = (float)velocityHistory[i];
        arrAlt[i] = (float)altitudeHistory[i];
        arrAcc[i] = (float)accelerationHistory[i];
        arrAtm[i] = (float)atmPressureHistory[i];
    }

    ImGui::PlotHistogram(i18n->t(telemetry_plots_velocity), arrVelo, n, 0, NULL, 0.0f, (float)maxVelo, ImVec2(370, 110.0f));
    ImGui::PlotHistogram(i18n->t(telemetry_plots_delta_v), arrAcc, n, 0, NULL, (float)minAcc, (float)maxAcc, ImVec2(370, 110.0f));
    ImGui::PlotHistogram(i18n->t(telemetry_plots_altitude), arrAlt, n, 0, NULL, 0.0f, (float)maxAlt, ImVec2(370, 110.0f));
    ImGui::PlotHistogram(i18n->t(telemetry_plots_atm_press), arrAtm, n, 0, NULL, 0.0f, (float)maxAtm, ImVec2(370, 110.0f));
    
    delete[] arrVelo;
    delete[] arrAlt;
    delete[] arrAcc;
    delete[] arrAtm;

    ImGui::End();
}

void Gui::renderPresentationModeInfo(double distance)
{    
    ImGui::SetNextWindowSize(ImVec2(450, 110), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(1250, 210), ImGuiCond_Once);
    ImGui::Begin(i18n->t(presentation_title));

    viewTelemetryPlot = false;
    viewCommands = false;
    viewSourceCode = false;
    viewFileOpen = false;
    
    ImGui::NewLine();

    std::string ssDistance = i18n->t(presentation_distance) + std::to_string(distance) + " km";
    ImGui::Text("%s", ssDistance.c_str());

    ImGui::End();
}

void Gui::renderTranslationErrors(ofsim_python_integration::PythonError &error)
{
    if (!error.active)
    {
        return;
    }
        
    ImGui::SetNextWindowSize(ImVec2(580, 110), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(500, 200), ImGuiCond_Once);

    ImGui::Begin(i18n->t(translation_errors_title));
    ImGui::TextWrapped("%s", error.message.c_str());
    
    ImGui::NewLine();

    if (ImGui::Button("OK", ImVec2(120, 0)))
    {
        error.active = false;
    }

    ImGui::End();
}

void Gui::restoreWindows()
{
    viewSimulationControl = true;
    viewTelemetryOn = true;
    viewTelemetryPlot = true;
    viewCommands = true;
    viewSourceCode = true;    
}

void Gui::renderCommandHistory(std::map<u64, RocketCommand>& commandHistory)
{
    if (!viewCommands) return;

    ImGui::SetNextWindowSize(ImVec2(450, 210), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(1290, 710), ImGuiCond_Once);

    ImGui::Begin(i18n->t(commands_title));

    std::map<u64, RocketCommand>::reverse_iterator itr;
    for (itr = commandHistory.rbegin(); itr != commandHistory.rend(); ++itr) 
    {
        u64 ctime = itr->first;
        u64 seconds = ctime / 1000;
        u64 millis = ctime % 1000;

        RocketCommand cmd = itr->second;
        
        RocketCommandCode code = cmd.code();
        f64 value = cmd.value();
        std::string cmdTxt = "";
        switch (code)
        {
            case RocketCommandCode::THRUST_MAGNITUDE_CHANGE:
                cmdTxt = i18n->t(commands_thrust_change);
                break;
            case RocketCommandCode::THRUST_ROTATION_CHANGE_X:
                cmdTxt = i18n->t(commands_thrust_direction_x);
                break;
            case RocketCommandCode::THRUST_ROTATION_CHANGE_Y:
                cmdTxt = i18n->t(commands_thrust_direction_y);
                break;
            case RocketCommandCode::THRUST_ROTATION_CHANGE_Z:
                cmdTxt = i18n->t(commands_thrust_direction_z);
                break;
            case RocketCommandCode::GYRO_ROTATION_CHANGE_X:
                cmdTxt = i18n->t(commands_thrust_rotation_x);
                break;
            case RocketCommandCode::GYRO_ROTATION_CHANGE_Y:
                cmdTxt = i18n->t(commands_thrust_rotation_y);
                break;
            case RocketCommandCode::GYRO_ROTATION_CHANGE_Z:
                cmdTxt = i18n->t(commands_thrust_rotation_z);
                break;
        }

        std::string message = std::to_string(seconds) + "." + std::to_string(millis) + "s: "
            + cmdTxt + " "
            + std::to_string(value);

        ImGui::Text("%s", message.c_str());
    }

    ImGui::End();
}

void Gui::renderDiagnostics(const DiagnosticsData& diagnostics)
{
    ImGui::SetNextWindowSize(ImVec2(450, 440), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(1050, 330), ImGuiCond_Once);

    ImGui::Begin("Simulation diagnostics: ");

    std::string positionTxt = "Rocket's position (x, y, z): \n"
        + std::to_string(diagnostics.rocketPosition.x) + ", \n"
        + std::to_string(diagnostics.rocketPosition.y) + ", \n"
        + std::to_string(diagnostics.rocketPosition.z) + ", \n";
    ImGui::Text("%s", positionTxt.c_str());

    std::string rotationTxt = "Rocket's rotation (x, y, z): \n"
        + std::to_string(diagnostics.rocketRotation.x) + ", \n"
        + std::to_string(diagnostics.rocketRotation.y) + ", \n"
        + std::to_string(diagnostics.rocketRotation.z);
    ImGui::Text("%s", rotationTxt.c_str());

    std::string anglesTxt = "Angles (dTheta, dPhi): \n"
        + std::to_string(diagnostics.dTheta) + ", \n"
        + std::to_string(diagnostics.dPhi) + ", \n";
    ImGui::Text("%s", anglesTxt.c_str());

    std::string thrustTxt = "Rocket's thrust vector direction (x, y, z): \n"
        + std::to_string(diagnostics.thrustVectorDirection.x) + ", \n"
        + std::to_string(diagnostics.thrustVectorDirection.y) + ", \n"
        + std::to_string(diagnostics.thrustVectorDirection.z);
    ImGui::Text("%s", thrustTxt.c_str());

    std::string cameraTxt = "Camera's position (x, y, x): \n"
            + std::to_string(diagnostics.cameraPosition.x) + ", \n"
            + std::to_string(diagnostics.cameraPosition.y) + ", \n"
            + std::to_string(diagnostics.cameraPosition.z);
    ImGui::Text("%s", cameraTxt.c_str());

    ImGui::End();
}

void ofsim_gui::Gui::renderManualControlData(const ofsim_simulation::ManualControlData& manualControl)
{
    ImGui::SetNextWindowSize(ImVec2(450, 780), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(60, 140), ImGuiCond_Once);

    ImGui::Begin("Manual mode: ");

    std::string rotationTxt = "Rocket's rotation (x, y, z): \n"
        + std::to_string(manualControl.rocketRotation.x) + ", \n"
        + std::to_string(manualControl.rocketRotation.y) + ", \n"
        + std::to_string(manualControl.rocketRotation.z);
    ImGui::Text("%s", rotationTxt.c_str());

    std::string velocityTxt = "Rocket's velocity (x, y, z): \n"
        + std::to_string(manualControl.velocityVector.x) + ", \n"
        + std::to_string(manualControl.velocityVector.y) + ", \n"
        + std::to_string(manualControl.velocityVector.z);
    ImGui::Text("%s", velocityTxt.c_str());

    std::string thrustTxt = "Rocket's thrust (x, y, z): \n"
        + std::to_string(manualControl.thrustVector.x) + ", \n"
        + std::to_string(manualControl.thrustVector.y) + ", \n"
        + std::to_string(manualControl.thrustVector.z);
    ImGui::Text("%s", thrustTxt.c_str());

    std::string thrustMagTxt = "Rocket's thrust vector magnitude: \n"
        + std::to_string(manualControl.thrustVectorMagnitude);
    ImGui::Text("%s", thrustMagTxt.c_str());

    ImGui::End();
}

void ofsim_gui::Gui::renderLoadingScreen()
{
    ImGui::SetNextWindowSize(ImVec2(380, 230), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(700, 270), ImGuiCond_Once);

    ImGui::Begin("Loading...");

    ImGui::SetWindowFontScale(1.8);
    ImGui::NewLine;
    ImGui::Text("%s", "Loading. Please wait...");

    ImGui::End();
}

void Gui::endRendering()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::loadTextures()
{    
    bool ret = ofsim_infrastructure::ImageLoader::loadTextureFromFile("assets/textures/play.png", &play_texture, &out_width, &out_height);
    IM_ASSERT(ret);
    ret = ofsim_infrastructure::ImageLoader::loadTextureFromFile("assets/textures/stop.png", &stop_texture, &out_width, &out_height);
    IM_ASSERT(ret);
    ret = ofsim_infrastructure::ImageLoader::loadTextureFromFile("assets/textures/pause.png", &pause_texture, &out_width, &out_height);
    IM_ASSERT(ret);
    ret = ofsim_infrastructure::ImageLoader::loadTextureFromFile("assets/textures/fwd.png", &fwd_texture, &out_width, &out_height);
    IM_ASSERT(ret);
    ret = ofsim_infrastructure::ImageLoader::loadTextureFromFile("assets/textures/splash.jpg", &splashTexture, &out_width, &out_height);
    IM_ASSERT(ret);

    pp_texture = play_texture;
}

void Gui::cleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

int Gui::getTimeFactor()
{
    if (timeFactor == -1) 
    {
        timeFactor = 0;
        return -1;
    }

    return timeFactor;
}
