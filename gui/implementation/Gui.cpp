#include "..\Gui.h"
#include "..\ImageUtils.h"

#include <iostream>

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

void Gui::renderSimulationControlWindow(unsigned __int64 time)
{
    ImGui::SetNextWindowSize(ImVec2(450, 100), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(100, 50), ImGuiCond_Once);

    ImGui::Begin("Kontrola symulacji: ");

    unsigned __int64 seconds = time / 1000;
    unsigned __int64 millis = time % 1000;

    std::string strClock = "Zegar: " + std::to_string(seconds) + "." + std::to_string(millis) + "s";
    ImGui::Text(strClock.c_str());

    ImGui::SameLine();
    std::string strTimeFactor = "Kompresja czasu: " + std::to_string(timeFactor) + "x";
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
        }
    } 
    
    ImGui::SameLine();

    if (ImGui::ImageButton((void*)(intptr_t)stop_texture, ImVec2(32, 32))) {
        std::cout << "Kliknieto stop!! \n";
        pp_texture = play_texture;
        plaing = false;
    }

    ImGui::SameLine();

    if (ImGui::ImageButton((void*)(intptr_t)fwd_texture, ImVec2(32, 32))) {
        std::cout << "Kliknieto fwd!! \n";

        if (timeFactor == 8)
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

void Gui::renderCodeEditor(static char text[])
{
    ImGui::SetNextWindowSize(ImVec2(450, 500), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(100, 200), ImGuiCond_Once);

    ImGui::Begin("Kod zrodlowy programu lotu:");

    //ImGui::InputTextMultiline()
    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    //static char text2[1024 * 16] = 

    ImGui::InputTextMultiline("##source", text, 1024*16, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 35), flags);

    ImGui::End();
}

void Gui::renderTelemetry(TelemetryData& telemetryData)
{
    ImGui::SetNextWindowSize(ImVec2(450, 120), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(100, 750), ImGuiCond_Once);

    ImGui::Begin("Telemetria:");
    
    std::string ssAltitude = "Wysokosc punktu widzenia: " + std::to_string(telemetryData.altitude) + " km";
    ImGui::Text(ssAltitude.c_str());

    std::string ssMass = "Masa rakiety: " + std::to_string(telemetryData.mass) + "t";
    ImGui::Text(ssMass.c_str());

    std::string ssPressure = "Cisnienie dynamiczne atmosfery: " + std::to_string(telemetryData.atmPressure * 10);
    ImGui::Text(ssPressure.c_str());

    std::string ssVelocity = "Predkosc bezwgledna rakiety: " + std::to_string(telemetryData.velocity) + "km/s";
    ImGui::Text(ssVelocity.c_str());

    std::string ssPosition = "Pozycja rakiety: (" + 
        std::to_string(telemetryData.position.x) + ", " +
        std::to_string(telemetryData.position.y) + ", " +
        std::to_string(telemetryData.position.z) + ")";
    ImGui::Text(ssPosition.c_str());

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::loadButtonTextures()
{    
    bool ret = ImageUtils::loadTextureFromFile("textures/play.png", &play_texture, &out_width, &out_height);
    IM_ASSERT(ret);
    ret = ImageUtils::loadTextureFromFile("textures/stop.png", &stop_texture, &out_width, &out_height);
    IM_ASSERT(ret);
    ret = ImageUtils::loadTextureFromFile("textures/pause.png", &pause_texture, &out_width, &out_height);
    IM_ASSERT(ret);
    ret = ImageUtils::loadTextureFromFile("textures/fwd.png", &fwd_texture, &out_width, &out_height);
    IM_ASSERT(ret);

    pp_texture = play_texture;
}

void Gui::cleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}