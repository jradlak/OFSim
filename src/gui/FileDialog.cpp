#include "FileDialog.h"

#include <iostream>

#include "../../external_libraries/imgui/imgui.h"
#include "../../external_libraries/imgui/imgui_impl_glfw.h"
#include "../../external_libraries/imgui/imgui_impl_opengl3.h"
#include "../../external_libraries/imgui/imgui_stdlib.h"

#include "../infrastructure/EventProcessor.h"
#include "../infrastructure/FileService.h"

#include "i18n.h"

using namespace ofsim_gui;
using namespace ofsim_events;

FileDialog::FileDialog() : i18n(I18n::getInstance())
{
    const char* homeDir = getenv("HOME");
    if (homeDir != nullptr)
    {
        directory = std::string(homeDir) + "/orb_progs";
    }
    else
    {
        directory = "/home";
    }

    std::cout << "Home directory: " << directory << std::endl;
}

void FileDialog::renderFileOpenDialog()
{
    EventProcessor* eventProcessor = EventProcessor::getInstance();

    viewFileOpen = true;

    ImGui::SetNextWindowSize(ImVec2(450, 410), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(600, 200), ImGuiCond_Once);

    ImGui::Begin(i18n->t(dialog_title));
    
    ImGui::Text("%s", i18n->t(dialog_directory));
    ImGui::InputText(" ", &directory);
    ImGui::SameLine();
    if (ImGui::Button(i18n->t(dialog_load), ImVec2(60, 0)))
    { 
        if (directory != "")
        { 
            loadFilesInDirectory(directory); 
        }
    }
    else 
    {
        if (directory != "")
        {
            loadFilesInDirectory(directory);
        }
    }

    ImGui::Separator();

    ImGui::Text("%s", i18n->t(dialog_file_list));
    const int items_count = filesInDirectory.size();
    const char* items[items_count];
    for (int i = 0; i < items_count; i++)
    {
        items[i] = filesInDirectory[i].name.c_str();
    }

    static int item_current = 0;
    ImGui::ListBox(" ", &item_current, items, items_count, 16);
    
    ImGui::Separator();

    if (ImGui::Button("OK", ImVec2(120, 0))) 
    {         
        std::string selectedFile = filesInDirectory[item_current].path + "/" + filesInDirectory[item_current].name;
        viewFileOpen = false; 
        eventProcessor->createEvent(StateEvent::PROGRAM_FILE_OPEN, selectedFile);               
    }

    ImGui::SetItemDefaultFocus();
    ImGui::SameLine();
    if (ImGui::Button(i18n->t(dialog_cancel), ImVec2(120, 0))) { viewFileOpen = false; }

    ImGui::End();
}

void FileDialog::loadFilesInDirectory(std::string &directory)
{    
    filesInDirectory = ofsim_infrastructure::loadfilesInDirectory(directory);
}
