#include "FileService.h"

#include <filesystem>
namespace fs = std::filesystem;

namespace ofsim_infrastructure
{
	void saveSourceCode(std::string &sourcePath, std::string &orbitalProgramSrc)
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

	std::string loadSourceCode(std::string &sourcePath)
	{
		std::string orbitalResult = "";
		
		std::ifstream sourceFile;
		sourceFile.open(sourcePath, std::ios::in);	

		if (sourceFile.is_open())
		{
			std::string line;

			while (std::getline(sourceFile, line))
			{
				orbitalResult += line + "\n";
			}

			sourceFile.close();
		}

		return orbitalResult;
	}

	std::vector<std::string> loadfilesInDirectory(std::string &dirName)
	{
		std::vector<std::string> filesInDirectory;
		if (fs::is_directory(dirName))
		{
			for (const auto& entry : fs::directory_iterator(dirName))
			{
				std::string fileName = entry.path().u8string();
				filesInDirectory.push_back(fileName);
			}
		}

		return filesInDirectory;
	}
}