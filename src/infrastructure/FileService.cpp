#include "FileService.h"

#include <filesystem>
#include <algorithm>

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
		
		FileDescriptor homeDir { FileType::HOME_DIRECTORY, ".", "." };
		filesInDirectory.push_back(homeDir);
		FileDescriptor parentDir { FileType::PARENT_DIRECTORY, "..", ".." };
		filesInDirectory.push_back(parentDir);

		if (fs::is_directory(dirName))
		{
			// at first, add directories:
			std::vector<FileDescriptor> directories;
			for (const auto& entry : fs::directory_iterator(dirName))
			{
				if (fs::is_directory(entry.path()))
				{
					FileDescriptor fileDescriptor;
					fileDescriptor.type = FileType::DIRECTORY;
					fileDescriptor.name = entry.path().filename().u8string() + "/";
					fileDescriptor.path = entry.path().parent_path().u8string();
					directories.push_back(fileDescriptor);
				}
			}
			// sort directories:
			std::sort(directories.begin(), directories.end(), [](const FileDescriptor& a, const FileDescriptor& b) {
				return a.name < b.name;
			});
			
			// then, add files:
			std::vector<FileDescriptor> files;
			for (const auto& entry : fs::directory_iterator(dirName))
			{
				if (fs::is_directory(entry.path()))
				{
					continue;
				}
				
				std::string filePath = entry.path().u8string();
				FileDescriptor fileDescriptor;
				fileDescriptor.type = FileType::FILE;
				fileDescriptor.name = entry.path().filename().u8string();
				fileDescriptor.path = entry.path().parent_path().u8string();
				files.push_back(fileDescriptor);
			}

			// sort files:
			std::sort(files.begin(), files.end(), [](const FileDescriptor& a, const FileDescriptor& b) {
				return a.name < b.name;
			});

			filesInDirectory.insert(filesInDirectory.end(), directories.begin(), directories.end());
			filesInDirectory.insert(filesInDirectory.end(), files.begin(), files.end());
		}

		return filesInDirectory;
	}
}