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

	std::string loadTextFile(std::string &sourcePath)
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

	std::vector<FileDescriptor> loadfilesInDirectory(std::string &dirName)
	{
		std::vector<FileDescriptor> filesInDirectory;
		if (fs::is_directory(dirName))
		{
			for (const auto& entry : fs::directory_iterator(dirName))
			{
				std::string filePath = entry.path().u8string();
				FileDescriptor fileDescriptor;
				fileDescriptor.name = entry.path().filename().u8string();
				fileDescriptor.path = entry.path().parent_path().u8string();
				filesInDirectory.push_back(fileDescriptor);
			}
		}

		return filesInDirectory;
	}
}