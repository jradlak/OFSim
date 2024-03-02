#include "FileService.h"

using namespace ofsim_infrastructure;

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

std::string FileService::loadSourceFile(std::string sourcePath)
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

// python integration: https://www.codeproject.com/Articles/820116/Embedding-Python-program-in-a-C-Cplusplus-code

void FileService::doSomePython()
{
	std::string hello = "print('Hello Python from C++!!!!!!!!!!!')";
	Py_Initialize();
	PyRun_SimpleString(hello.c_str());
	Py_Finalize();
}
