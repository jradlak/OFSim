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
