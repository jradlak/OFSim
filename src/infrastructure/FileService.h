#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace ofsim_infrastructure
{
	enum class FileType
	{
		HOME_DIRECTORY,
		PARENT_DIRECTORY,
		DIRECTORY,
		FILE
	};

	struct FileDescriptor
	{
		FileType type;
		std::string name;
		std::string path;
	};

	void saveSourceCode(std::string &sourcePath, std::string &orbitalProgramSrc);
	std::string loadTextFile(std::string &sourcePath);
	std::vector<FileDescriptor> loadfilesInDirectory(std::string& dirName);
}