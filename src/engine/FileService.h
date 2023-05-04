#pragma once

#include <fstream>
#include <iostream>
#include <string>

class FileService
{
public:
	static void saveSourceCode(std::string sourcePath, std::string orbitalProgramSrc);
};

