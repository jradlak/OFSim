#include "PythonMachine.h"

#include "../infrastructure/EventProcessor.h"

#include <stdexcept>

PyObject *ofsim_python_integration::PythonMachine::orbital_thrust_magnitude_change(PyObject *self, PyObject *args)
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

PyObject *ofsim_python_integration::PythonMachine::orbital_rocket_rotation_change(PyObject *self, PyObject *args)
{
    PyObject* listObj;

    if (!PyArg_ParseTuple( args, "O", &listObj))
    {
        return NULL;
    }

    PyObject* valueX = PyList_GetItem(listObj, 0);
    double x = PyFloat_AsDouble(valueX);
    PyObject* valueY = PyList_GetItem(listObj, 1);
    double y = PyFloat_AsDouble(valueY);
    PyObject* valueZ = PyList_GetItem(listObj, 2);
    double z = PyFloat_AsDouble(valueZ);

    dvec3 deltaRotation {x, y, z};
    ofsim_events::EventProcessor::getInstance()->changeThrustRotatation(deltaRotation);

    //Py_DecRef(listObj);

    return PyLong_FromLong(0);
}

PyObject *ofsim_python_integration::PythonMachine::get_orbital_data(PyObject *self, PyObject *argsB)
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
    // Register once; the interpreter is initialized/finalized for each run.
    static const int registered = PyImport_AppendInittab("orbital_fs", &PyInit_orbital_fs);
    if (registered != 0) throw std::runtime_error("Unable to register orbital_fs");
    CPyInstance hInstance;

    PyObject* globals = PyDict_New();
    if (globals == nullptr) throw std::runtime_error("Unable to create Python globals");
    PyObject* result = PyRun_String(sourceCode.c_str(), Py_file_input, globals, globals);
    Py_DECREF(globals);
    if (result == NULL)
    {
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);
        
        PyObject *str_exc = value ? PyObject_Str(value) : nullptr;
        const char* text = str_exc ? PyUnicode_AsUTF8(str_exc) : nullptr;
        std::string error_message = text ? text : "Unknown Python script error";

        Py_XDECREF(str_exc);
        Py_XDECREF(type);
        Py_XDECREF(value);
        Py_XDECREF(traceback);
        // ProgramRunner transfers this error after Python has finalized.
        throw std::runtime_error(error_message);
    }
    Py_DECREF(result);
}

void ofsim_python_integration::PythonMachine::terminateProgram()
{
    ofsim_events::EventProcessor::getInstance()->terminatePythonMachine(true);
}
