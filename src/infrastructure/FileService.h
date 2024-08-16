#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace ofsim_infrastructure
{
	void saveSourceCode(std::string &sourcePath, std::string &orbitalProgramSrc);
	std::string loadSourceCode(std::string &sourcePath);
	std::vector<std::string> loadfilesInDirectory(std::string& dirName);
}