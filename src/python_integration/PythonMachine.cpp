#include "PythonMachine.h"

// python integration: https://www.codeproject.com/Articles/820116/Embedding-Python-program-in-a-C-Cplusplus-code

PyObject *ofsim_python_integration::PythonMachine::orbital_thrust_change(PyObject *self, PyObject *args)
{
    PyObject *a;
    if (PyArg_UnpackTuple(args, "", 1, 1, &a))
    {        
        double thrust = PyFloat_AsDouble(a);
        ofsim_events::EventProcessor::getInstance()->setRocketValue(thrust);
        std::cout << "orbital_thrust_change: " << thrust << std::endl;
    }

    return PyLong_FromLong(0);
}

PyObject *ofsim_python_integration::PythonMachine::orbital_thrust_get(PyObject *self, PyObject *args)
{
    u64 thrust = ofsim_events::EventProcessor::getInstance()->getRocketValue();

    PyObject* thrust_vector = PyDict_New();
    PyDict_SetItemString(thrust_vector, "thrust", PyFloat_FromDouble(thrust));
    PyDict_SetItemString(thrust_vector, "angle", PyFloat_FromDouble(thrust * 3.14));

    return thrust_vector;
}

void ofsim_python_integration::PythonMachine::runPythonOrbitalProgram(std::string sourceCode)
{
    PyImport_AppendInittab("orbital_fs", &PyInit_orbital_fs);
    CPyInstance hInstance;

    PyRun_SimpleString(sourceCode.c_str());
}