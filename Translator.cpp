#include "Translator.h"

static inline std::string& ltrim(std::string& s);
static inline std::string& rtrim(std::string& s);
static inline std::string& trim(std::string& s);

void Translator::translate(const char* sourcePath)
{
    std::ifstream sourceFile;
    
    std::string sourceCode = "";

    sourceFile.open(sourcePath, std::ios::in);
        
    if (sourceFile.is_open()) {
        std::string line;
        
        while (std::getline(sourceFile, line))
        {
            findLabel(line);
        }

        //std::cout << "Wlasciwa translacja: \n";

        sourceFile.clear();
        sourceFile.seekg(0);

        while (std::getline(sourceFile, line))
        {
            translate(line);
        }

        sourceFile.close();
    }
}

void Translator::translate(std::string sourceLine)
{
    //std::cout << sourceLine << "\n";
}

void Translator::findLabel(std::string sourceLine)
{
    std::string line = trim(sourceLine);
    if ((int)line.rfind(":") > 0 && (int)line.rfind(";") < 0) {
        //std::cout << "label: " << line << "\n";
    }
}

// code below based on: https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
// trim from start
static inline std::string& ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string& rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
        std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string& trim(std::string& s) {
    return ltrim(rtrim(s));
}
