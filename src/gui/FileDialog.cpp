#include "FileDialog.h"

#include <iostream>
#include <filesystem>

#include "../../external_libraries/imgui/imgui.h"
#include "../../external_libraries/imgui/imgui_impl_glfw.h"
#include "../../external_libraries/imgui/imgui_impl_opengl3.h"
#include "../../external_libraries/imgui/imgui_stdlib.h"

#include "../infrastructure/EventProcessor.h"
#include "../infrastructure/FileService.h"

#include "i18n.h"

using namespace ofsim_gui;
using namespace ofsim_events;

FileDialog::FileDialog(DialogMode dialogMode) : i18n(I18n::getInstance()), mode(dialogMode)
{
    const char* homeDir = getenv("HOME");
    if (homeDir != nullptr)
    {
        directory = std::string(homeDir);
    }
    else
    {
        directory = "/home";
    }

    std::cout << "Home directory: " << directory << std::endl;
}

void FileDialog::renderFileDialog()
{
    EventProcessor* eventProcessor = EventProcessor::getInstance();

    viewFileOpen = true;

    ImGui::SetNextWindowSize(ImVec2(480, 410), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(600, 220), ImGuiCond_Once);

    ImGui::Begin(i18n->t(dialog_title));
    
    ImGui::Text("%s", i18n->t(dialog_directory));
    ImGui::InputText(" ", &directory);
    ImGui::SameLine();
    if (ImGui::Button(i18n->t(dialog_load), ImVec2(120, 0)))
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

    const auto& selectedItem = filesInDirectory[item_current];
    const char* fileBtnName = mode == DialogMode::FILE_OPEN ? i18n->t(dialog_load_file_directory)
                                : i18n->t(dialog_load_directory);
                                
    if (ImGui::Button((fileBtnName), ImVec2(160, 0))) 
    {  
        if (selectedItem.type == ofsim_infrastructure::FileType::DIRECTORY)
        {
            directory = selectedItem.path + "/" + selectedItem.name;
            loadFilesInDirectory(directory);
        }
        else if (selectedItem.type == ofsim_infrastructure::FileType::FILE)
        {
            if (mode == DialogMode::FILE_OPEN)
            {
                std::string selectedFile = selectedItem.path + "/" + selectedItem.name;
                viewFileOpen = false; 
                eventProcessor->createEvent(StateEvent::PROGRAM_FILE_OPEN, selectedFile);    
            }                   
        } 
        else if (selectedItem.type == ofsim_infrastructure::FileType::HOME_DIRECTORY)
        {
            directory = getenv("HOME");
            loadFilesInDirectory(directory);
        } 
        else if (selectedItem.type == ofsim_infrastructure::FileType::PARENT_DIRECTORY)
        {
            directory = std::filesystem::path(directory).parent_path().u8string();
            loadFilesInDirectory(directory);
        }
    }
    
    ImGui::SetItemDefaultFocus();
    ImGui::SameLine();
    if (mode == DialogMode::FILE_SAVE)
    {
        if (ImGui::Button((i18n->t(dialog_save_file)), ImVec2(160, 0))) 
        { 
            viewFileOpen = false; 
            eventProcessor->createEvent(StateEvent::FILE_SAVED_AS, ""); 
        }

        ImGui::SameLine();
    }

    if (ImGui::Button(i18n->t(dialog_cancel), ImVec2(120, 0))) 
    { 
        viewFileOpen = false; 
    }

    ImGui::End();
}

void FileDialog::loadFilesInDirectory(std::string &directory)
{    
    filesInDirectory = ofsim_infrastructure::loadfilesInDirectory(directory);
}
