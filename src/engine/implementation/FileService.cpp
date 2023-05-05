#include "../FileService.h"

void FileService::saveSourceCode(std::string sourcePath, std::string orbitalProgramSrc)
{
	if (orbitalProgramSrc != "")
	{
		std::ofstream destFile;
		destFile.open(sourcePath, std::ios::out | std::ios::trunc);

		if (destFile.is_open())
		{
			destFile << orbitalProgramSrc;
		}

		destFile.close();
	}
}

std::string FileService::loadSourceFile(std::string sourcePath)
{
	std::ifstream sourceFile;

	sourceFile.open(sourcePath.c_str(), std::ios::in);

	std::string orbitalProgramSourceCode = "";
	if (sourceFile.is_open()) {
		std::string line;

		while (sourceFile)
		{
			std::getline(sourceFile, line, '\r');
			sourceFile >> line;
			orbitalProgramSourceCode += line + "\n";
		}

		sourceFile.close();
	}

	return orbitalProgramSourceCode;
}
