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

void Gui::renderSimulationControlWindow()
{
    ImGui::SetNextWindowSize(ImVec2(450, 100), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(100, 50), ImGuiCond_Once);

    ImGui::Begin("Kontrola symulacji:");

    if (ImGui::ImageButton((void*)(intptr_t)out_texture, ImVec2(50, 50))) {
        std::cout << "Kliknieto!! \n";
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

void Gui::loadButtonTexture()
{    
    bool ret = ImageUtils::loadTextureFromFile("textures/fruits.png", &out_texture, &out_width, &out_height);
    IM_ASSERT(ret);
}

void Gui::cleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}