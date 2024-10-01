#pragma once

#include <string>
#include <vector>
#include "i18n.h"
#include "../infrastructure/FileService.h"

namespace ofsim_gui
{
    enum class DialogMode
    {
        FILE_OPEN,
        FILE_SAVE
    }; 

    class FileDialog
    {
    public:
        FileDialog(DialogMode mode);

        void renderFileDialog();

        bool viewFileOpen { false };

    private:
        std::string directory { "" };    
        std::vector<ofsim_infrastructure::FileDescriptor> filesInDirectory;
		DialogMode mode;

        I18n* i18n;

        void loadFilesInDirectory(std::string &directory);	
    };
}
