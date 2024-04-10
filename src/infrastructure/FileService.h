#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <Python.h>

namespace ofsim_infrastructure
{
	void saveSourceCode(std::string sourcePath, std::string orbitalProgramSrc);
	std::string loadSourceCode(std::string sourcePath);
}