#include "..\Gui.h"

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

void Gui::render()
{
    // render imgui:
    ImGui::Begin("My name is Window, ImGUI Window");
    ImGui::Text("Hello there adventurer!");
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::renderTelemetry(TelemetryData& telemetryData)
{

}

void Gui::cleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}