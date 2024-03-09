#include "PythonMachine.h"

// python integration: https://www.codeproject.com/Articles/820116/Embedding-Python-program-in-a-C-Cplusplus-code

PyObject *ofsim_python_integration::PythonMachine::orbital_thrust_change(PyObject *self, PyObject *args)
{
    PyObject *a;
    if (PyArg_UnpackTuple(args, "", 1, 1, &a))
    {        
        double thrust = PyFloat_AsDouble(a);
        ofsim_events::EventProcessor::getInstance()->setThrustMagnitude(thrust);
        std::cout << "orbital_thrust_change: " << thrust << std::endl;
    }

    return PyLong_FromLong(0);
}

PyObject *ofsim_python_integration::PythonMachine::get_orbital_data(PyObject *self, PyObject *args)
{
    PyObject* rocket_data = PyDict_New();
    
    dvec3 position = ofsim_events::EventProcessor::getInstance()->getRocketPosition();    
    PyDict_SetItemString(rocket_data, "positionX", PyFloat_FromDouble(position.x));
    PyDict_SetItemString(rocket_data, "positionY", PyFloat_FromDouble(position.y));
    PyDict_SetItemString(rocket_data, "positionZ", PyFloat_FromDouble(position.z));

    dvec3 velocity = ofsim_events::EventProcessor::getInstance()->getRocketVelocity();
    PyDict_SetItemString(rocket_data, "velocityX", PyFloat_FromDouble(velocity.x));
    PyDict_SetItemString(rocket_data, "velocityY", PyFloat_FromDouble(velocity.y));
    PyDict_SetItemString(rocket_data, "velocityZ", PyFloat_FromDouble(velocity.z));

    dvec3 rotation = ofsim_events::EventProcessor::getInstance()->getRocketRotation();
    PyDict_SetItemString(rocket_data, "rotationX", PyFloat_FromDouble(rotation.x));
    PyDict_SetItemString(rocket_data, "rotationY", PyFloat_FromDouble(rotation.y));
    PyDict_SetItemString(rocket_data, "rotationZ", PyFloat_FromDouble(rotation.z));

    f64 altitude = ofsim_events::EventProcessor::getInstance()->getAltitude();
    PyDict_SetItemString(rocket_data, "altitude", PyFloat_FromDouble(altitude));

    f64 thrustMagnitude = ofsim_events::EventProcessor::getInstance()->getThrustMagnitude();
    PyDict_SetItemString(rocket_data, "thrustMagnitude", PyFloat_FromDouble(thrustMagnitude));

    f64 mass = ofsim_events::EventProcessor::getInstance()->getRocketMass();
    PyDict_SetItemString(rocket_data, "rocketMass", PyFloat_FromDouble(mass));

    bool isTerminated = ofsim_events::EventProcessor::getInstance()->isPythonMachineTerminated();
    PyDict_SetItemString(rocket_data, "shouldStop", PyBool_FromLong(isTerminated));
    
    return rocket_data;
}

void ofsim_python_integration::PythonMachine::runPythonOrbitalProgram(std::string sourceCode)
{
    PyImport_AppendInittab("orbital_fs", &PyInit_orbital_fs);
    CPyInstance hInstance;

    PyRun_SimpleString(sourceCode.c_str());
}

void ofsim_python_integration::PythonMachine::terminateProgram()
{
    ofsim_events::EventProcessor::getInstance()->terminatePythonMachine(true);
}