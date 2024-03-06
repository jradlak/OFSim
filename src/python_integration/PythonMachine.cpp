#include "PythonMachine.h"

// python integration: https://www.codeproject.com/Articles/820116/Embedding-Python-program-in-a-C-Cplusplus-code

void ofsim_python_integration::PythonMachine::runPythonOrbitalProgram(std::string sourceCode)
{
    PyImport_AppendInittab("orbital_fs", &PyInit_orbital_fs);
	CPyInstance hInstance;

    PyRun_SimpleString(sourceCode.c_str());
}