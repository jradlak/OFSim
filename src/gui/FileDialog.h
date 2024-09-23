#pragma once

#include <string>
#include <vector>
#include "i18n.h"

namespace ofsim_gui
{
    class FileDialog
    {
    public:
        FileDialog();

        void renderFileOpenDialog();

        bool viewFileOpen { false };

    private:
        std::string directory { "/home/jakub/orb_progs" };    
        std::vector<std::string> filesInDirectory;
		
        I18n* i18n;

        void loadFilesInDirectory(std::string &directory);	
    };
}
