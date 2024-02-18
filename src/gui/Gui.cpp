#include "Gui.h"
#include "../infrastructure/ImageLoader.h"

#include <iostream>

using namespace ofsim_gui;

void Gui::initialization(Window* mainWindow)
{
    // ImGui initialization:
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(mainWindow->getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Gui::newFrame()
{     
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::renderMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Program"))
        {
            if (ImGui::MenuItem(i18n->t(menu_new))) 
            { 
                createUserEvent(UserClickAction::FILE_NEW, ""); 
            }

            if (ImGui::MenuItem(i18n->t(menu_open))) 
            {                 
                viewFileOpen = true;
                createUserEvent(UserClickAction::FILE_OPEN, "");                
            }

            if (ImGui::MenuItem(i18n->t(menu_save))) 
            { 
                createUserEvent(UserClickAction::FILE_SAVE, "");
            }

            if (ImGui::MenuItem(i18n->t(menu_save_as)))
            {
                viewFileSaveAs = true;                
            }
            
            ImGui::Separator();
            if (ImGui::MenuItem(i18n->t(menu_close))) 
            {  
                createUserEvent(UserClickAction::FILE_EXIT, "");
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
                createUserEvent(UserClickAction::HELP_HELP, "");               
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem(i18n->t(menu_about))) 
            { 
                createUserEvent(UserClickAction::HELP_ABOUT, "");
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

    ImGui::TextWrapped(i18n->t(splash_info));

    ImGui::NewLine();
   
    ImGui::Separator();

    ImGui::Text(i18n->t(splash_version));
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

void Gui::renderFileSaveAsDialog()
{
    if (!viewFileSaveAs) return;

    ImGui::SetNextWindowSize(ImVec2(380, 110), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(600, 200), ImGuiCond_Once);

    ImGui::Begin(i18n->t(dialog_save_title));

    ImGui::Text(i18n->t(dialog_save_file_name));
    ImGui::InputText(" ", &savedFile);

    if (ImGui::Button("OK", ImVec2(120, 0)))
    {        
        viewFileSaveAs = false;
        createUserEvent(UserClickAction::FILE_SAVED_AS, "");
    }

    ImGui::SetItemDefaultFocus();
    ImGui::SameLine();

    if (ImGui::Button(i18n->t(dialog_cancel), ImVec2(120, 0))) { viewFileSaveAs = false; }

    ImGui::End();
}

void Gui::renderFileOpenDialog()
{   
    if (!viewFileOpen) return;

    ImGui::SetNextWindowSize(ImVec2(450, 210), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(600, 200), ImGuiCond_Once);

    ImGui::Begin(i18n->t(dialog_title));
    
    ImGui::Text(i18n->t(dialog_directory));
    ImGui::InputText(" ", &directory);
    ImGui::SameLine();
    if (ImGui::Button(i18n->t(dialog_load), ImVec2(60, 0)))
    { 
        if (directory != "")
        { 
            loadFilesInDirectory(directory); 
        }
    }

    ImGui::Separator();

    ImGui::Text(i18n->t(dialog_file_list));
    const char* items[10];
    for (int i = 0; i < filesInDirectory.size(); i++)
    {
        items[i] = filesInDirectory[i].c_str();
    }

    static int item_current = 0;
    ImGui::ListBox(" ", &item_current, items, filesInDirectory.size(), 4);
    
    ImGui::Separator();

    if (ImGui::Button("OK", ImVec2(120, 0))) 
    {         
        std::string selectedFile = filesInDirectory[item_current];
        viewFileOpen = false; 
        createUserEvent(UserClickAction::PROGRAM_FILE_OPENED, selectedFile);
    }

    ImGui::SetItemDefaultFocus();
    ImGui::SameLine();
    if (ImGui::Button(i18n->t(dialog_cancel), ImVec2(120, 0))) { viewFileOpen = false; }

    ImGui::End();
}

void Gui::renderSimulationControlWindow(unsigned long long time)
{
    if (!viewSimulationControl) return;

    ImGui::SetNextWindowSize(ImVec2(450, 100), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(100, 50), ImGuiCond_Once);

    ImGui::Begin(i18n->t(sim_panel_title));

    unsigned long long seconds = time / 1000;
    unsigned long long millis = time % 1000;

    std::string strClock = i18n->t(sim_panel_clock) + std::to_string(seconds) + "." + std::to_string(millis) + "s";
    ImGui::Text(strClock.c_str());

    ImGui::SameLine();
    std::string strTimeFactor = i18n->t(sim_panel_time_compression) + std::to_string(timeFactor) + "x";
    ImGui::Text(strTimeFactor.c_str());

    if (ImGui::ImageButton((void*)(intptr_t)pp_texture, ImVec2(32, 32))) {
        if (plaing)
        {
            std::cout << "Kliknieto pause!! \n";
            pp_texture = play_texture;
            plaing = false;
            timeFactor = 0;
        }
        else
        {
            std::cout << "Kliknieto play!! \n";
            pp_texture = pause_texture;
            plaing = true;
            timeFactor = 1;

            createUserEvent(UserClickAction::PROGRAM_START_EXECUTION, "");
        }
    }
    
    ImGui::SameLine();

    if (ImGui::ImageButton((void*)(intptr_t)stop_texture, ImVec2(32, 32))) {
        std::cout << "Kliknieto stop!! \n";
        pp_texture = play_texture;
        plaing = false;
        timeFactor = -1;
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

    ImGui::End();
}

void Gui::renderCodeEditor(std::string& text)
{
    if (!viewSourceCode) return;

    ImGui::SetNextWindowSize(ImVec2(450, 780), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(100, 160), ImGuiCond_Once);
    
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

    ImGui::EndChild();

    ImGui::End();
}

void Gui::renderTelemetry(TelemetryData& telemetryData)
{
    if (!viewTelemetryOn) return;

    ImGui::SetNextWindowSize(ImVec2(450, 150), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(1250, 50), ImGuiCond_Once);

    ImGui::Begin(i18n->t(telemetry_title));
    
    std::string ssAltitude = i18n->t(telemetry_altitude) + std::to_string(telemetryData.altitude) + " km";
    ImGui::Text(ssAltitude.c_str());

    std::string ssMass = i18n->t(telemetry_mass) + std::to_string(telemetryData.mass) + "t";
    ImGui::Text(ssMass.c_str());

    std::string ssPressure = i18n->t(telemetry_atm_press) + std::to_string(telemetryData.atmPressure * 10);
    ImGui::Text(ssPressure.c_str());

    std::string ssVelocity = i18n->t(telemetry_velocity) + std::to_string(telemetryData.velocity) + "km/s";
    ImGui::Text(ssVelocity.c_str());

    std::string ssPosition = i18n->t(telemetry_position) +
        std::to_string(telemetryData.position.x) + ", " +
        std::to_string(telemetryData.position.y) + ", " +
        std::to_string(telemetryData.position.z) + ")";
    ImGui::Text(ssPosition.c_str());

    std::string ssApogeum = i18n->t(telemetry_apogee);
    if (telemetryData.apogeum != 0)
    {
        ssApogeum = i18n->t(telemetry_apogee) + std::to_string(telemetryData.apogeum) + " km";
    }

    std::string ssPerygeum = i18n->t(telemetry_perigee);
    if (telemetryData.perygeum != 0)
    {
        ssPerygeum = i18n->t(telemetry_perigee) + std::to_string(telemetryData.perygeum) + " km";
    }

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ssApogeum.c_str());
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), ssPerygeum.c_str());
    
    ImGui::End();
}

void Gui::endRendering()
{    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::plotTelemetry(
    std::vector<double> velocityHistory, double maxVelo,
    std::vector<double> altitudeHistory, double maxAlt,
    std::vector<double> atmPressureHistory, double maxAtm,
    std::vector<double> accelerationHistory, double maxAcc, double minAcc)
{
    if (!viewTelemetryPlot) return;

    ImGui::SetNextWindowSize(ImVec2(450, 510), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(1250, 210), ImGuiCond_Once);
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
    ImGui::Text(ssDistance.c_str());

    ImGui::End();
}

void Gui::renderTranslationErrors(std::string errors)
{
    if (errors == "") return;
    
    ImGui::SetNextWindowSize(ImVec2(380, 110), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(600, 200), ImGuiCond_Once);

    ImGui::Begin(i18n->t(translation_errors_title));
    ImGui::Text(errors.c_str());
    
    ImGui::NewLine();


    if (ImGui::Button("OK", ImVec2(120, 0)))
    {
        
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

void Gui::renderCommandHistory(std::map<unsigned long long, RocketCommand>& commandHistory)
{
    if (!viewCommands) return;

    ImGui::SetNextWindowSize(ImVec2(450, 210), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(1250, 730), ImGuiCond_Once);

    ImGui::Begin(i18n->t(commands_title));

    std::map<unsigned long long, RocketCommand>::reverse_iterator itr;
    for (itr = commandHistory.rbegin(); itr != commandHistory.rend(); ++itr) 
    {
        unsigned long long ctime = itr->first;
        unsigned long long seconds = ctime / 1000;
        unsigned long long millis = ctime % 1000;

        RocketCommand cmd = itr->second;
        
        int code = cmd.code();
        double value = cmd.value();
        std::string cmdTxt = "";
        switch (code)
        {
            case 1:
                cmdTxt = i18n->t(commands_thrust_change);
                break;
            case 2:
                cmdTxt = i18n->t(commands_thrust_direction_x);
                break;
            case 3:
                cmdTxt = i18n->t(commands_thrust_direction_y);
                break;
            case 4:
                cmdTxt = i18n->t(commands_thrust_direction_z);
                break;
            case 5:
                cmdTxt = i18n->t(commands_thrust_rotation_x);
                break;
            case 6:
                cmdTxt = i18n->t(commands_thrust_rotation_y);
                break;
            case 7:
                cmdTxt = i18n->t(commands_thrust_rotation_z);
                break;
        }

        std::string message = std::to_string(seconds) + "." + std::to_string(millis) + "s: "
            + cmdTxt + " "
            + std::to_string(value);

        ImGui::Text(message.c_str());
    }

    ImGui::End();
}

void Gui::renderDiagnostics(glm::dvec3 position, glm::dvec3 rotation)
{
    ImGui::SetNextWindowSize(ImVec2(450, 240), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(1050, 330), ImGuiCond_Once);

    ImGui::Begin("Diagnostyka obiektów:");
    std::string positionTxt = "Position (x, y, z): "
        + std::to_string(position.x) + ", "
        + std::to_string(position.y) + ", "
        + std::to_string(position.z);
    
    std::string rotationTxt = "Rotation (x, y, z): "
        + std::to_string(rotation.x) + ", "
        + std::to_string(rotation.y) + ", "
        + std::to_string(rotation.z);

    ImGui::Text(positionTxt.c_str());
    ImGui::Text(rotationTxt.c_str());

    ImGui::End();
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

UserEvent ofsim_gui::Gui::getUserEvent()
{
    if (userEvent == nullptr)
    {
         return UserEvent();
    }

    UserEvent event = UserEvent(userEvent->id, userEvent->timestamp, userEvent->action, userEvent->data);
    delete userEvent;
    userEvent = nullptr;

    return event;
}

void Gui::loadFilesInDirectory(std::string &directory)
{
    filesInDirectory.clear();
    if (fs::is_directory(directory)) 
    {
        for (const auto& entry : fs::directory_iterator(directory))
        {
            std::string fileName = entry.path().u8string();
                std::cout << fileName << std::endl;
                filesInDirectory.push_back(fileName);
        }
    }
}

void ofsim_gui::Gui::createUserEvent(UserClickAction action, std::string data)
{
    if (userEvent == nullptr)
    {
        userEvent = new UserEvent(eventCounter++, currentTime(), action, data);
    }
}

u64 ofsim_gui::Gui::currentTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();  
}