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
    ImGui::SetNextWindowPos(ImVec2(100, 110), ImGuiCond_Once);

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
    ImGui::SetNextWindowSize(ImVec2(450, 750), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(100, 220), ImGuiCond_Once);

    ImGui::Begin("Kod zrodlowy programu lotu:");

    //ImGui::InputTextMultiline()
    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    //static char text2[1024 * 16] = 

    ImGui::InputTextMultiline("##source", &text, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 55), flags);

    ImGui::End();
}

void Gui::renderTelemetry(TelemetryData& telemetryData)
{
    ImGui::SetNextWindowSize(ImVec2(450, 150), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(1250, 50), ImGuiCond_Once);

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

    std::string ssApogeum = "Apogeum orbity: ";
    if (telemetryData.apogeum != 0)
    {
        ssApogeum = "Apogeum orbity: " + std::to_string(telemetryData.apogeum) + " km";
    }

    std::string ssPerygeum = "Perygeum orbity: ";
    if (telemetryData.perygeum != 0)
    {
        ssPerygeum = "Perygeum orbity: " + std::to_string(telemetryData.perygeum) + " km";
    }

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ssApogeum.c_str());
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), ssPerygeum.c_str());

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::plotTelemetry(
    std::vector<double> velocityHistory, double maxVelo,
    std::vector<double> altitudeHistory, double maxAlt,
    std::vector<double> atmPressureHistory, double maxAtm,
    std::vector<double> accelerationHistory, double maxAcc, double minAcc)
{
    ImGui::SetNextWindowSize(ImVec2(450, 510), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(1250, 210), ImGuiCond_Once);
    ImGui::Begin("Wykresy telemetrii:");

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

    ImGui::PlotHistogram("Predkosc", arrVelo, n, 0, NULL, 0.0f, (float)maxVelo, ImVec2(370, 110.0f));
    ImGui::PlotHistogram("Przysp.", arrAcc, n, 0, NULL, (float)minAcc, (float)maxAcc, ImVec2(370, 110.0f));
    ImGui::PlotHistogram("Wysokosc", arrAlt, n, 0, NULL, 0.0f, (float)maxAlt, ImVec2(370, 110.0f));
    ImGui::PlotHistogram("Cisn. atm.", arrAtm, n, 0, NULL, 0.0f, (float)maxAtm, ImVec2(370, 110.0f));
    
    delete[] arrVelo;
    delete[] arrAlt;
    delete[] arrAcc;
    delete[] arrAtm;

    ImGui::End();
}

void Gui::renderCommandHistory(std::map<unsigned __int64, RocketCommand>& commandHistory)
{
    ImGui::SetNextWindowSize(ImVec2(450, 240), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(1250, 735), ImGuiCond_Once);

    ImGui::Begin("Wykonane komendy:");

    std::map<unsigned __int64, RocketCommand>::reverse_iterator itr;
    for (itr = commandHistory.rbegin(); itr != commandHistory.rend(); ++itr) 
    {
        unsigned __int64 ctime = itr->first;
        unsigned __int64 seconds = ctime / 1000;
        unsigned __int64 millis = ctime % 1000;

        RocketCommand cmd = itr->second;
        
        int code = cmd.code();
        double value = cmd.value();
        std::string cmdTxt = "";
        switch (code)
        {
            case 1:
                cmdTxt = "zmiana wartosci ciagu: ";
                break;
            case 2:
                cmdTxt = "zmiana kierunku ciagu w osi X: ";
                break;
            case 3:
                cmdTxt = "zmiana kierunku ciagu w osi Y: ";
                break;
            case 4:
                cmdTxt = "zmiana kierunku cigu w osi Z: ";
                break;
            case 5:
                cmdTxt = "zmiana rotacji zyroskopowej w osi X ";
                break;
            case 6:
                cmdTxt = "zmiana rotacji zyroskopowej w osi Y ";
                break;
            case 7:
                cmdTxt = "zmiana rotacji zyroskopowej w osi Z ";
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

int Gui::getTimeFactor()
{
    if (timeFactor == -1) 
    {
        timeFactor = 0;
        return -1;
    }

    return timeFactor;
}
