#pragma once

#include <fstream>
#include <iostream>
#include <string>

namespace ofs_infrastructure
{
	struct FileService
	{		
		static void saveSourceCode(std::string sourcePath, std::string orbitalProgramSrc);

		static std::string loadSourceFile(std::string sourcePath);	
	};
}