#include "Translator.h"

void Translator::translate(const char* sourcePath)
{
    std::ifstream sourceFile;
    
    std::string sourceCode = "";

    sourceFile.open(sourcePath, std::ios::in);
        
    if (sourceFile.is_open()) {
        std::string line;
        while (std::getline(sourceFile, line))
        {
            translate(line);
        }

        sourceFile.close();
    }
}

void Translator::translate(std::string sourceLine)
{
    std::cout << sourceLine << "\n";
}
