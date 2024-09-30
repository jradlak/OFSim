#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace ofsim_infrastructure
{
	struct FileDescriptor
	{
		std::string name;
		std::string path;
	};

	void saveSourceCode(std::string &sourcePath, std::string &orbitalProgramSrc);
	std::string loadTextFile(std::string &sourcePath);
	std::vector<FileDescriptor> loadfilesInDirectory(std::string& dirName);
}