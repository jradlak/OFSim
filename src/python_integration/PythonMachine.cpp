#include "PythonMachine.h"

#include "../infrastructure/EventProcessor.h"

#include <stdexcept>

PyObject* ofsim_python_integration::PythonMachine::orbital_thrust_magnitude_change(PyObject* self,
                                                                                   PyObject* args)
{
    PyObject* a;
    if (PyArg_UnpackTuple(args, "", 1, 1, &a))
    {
        double thrust = PyFloat_AsDouble(a);
        try
        {
            const auto runId = *static_cast<u64*>(PyModule_GetState(self));
            ofsim_events::EventProcessor::getInstance()->setThrustMagnitude(runId, thrust);
        }
        catch (const std::exception& error)
        {
            PyErr_SetString(PyExc_RuntimeError, error.what());
            return nullptr;
        }
        std::cout << "orbital_thrust_change: " << thrust << std::endl;
    }

    return PyLong_FromLong(0);
}

PyObject* ofsim_python_integration::PythonMachine::orbital_rocket_rotation_change(PyObject* self,
                                                                                  PyObject* args)
{
    PyObject* listObj;

    if (!PyArg_ParseTuple(args, "O", &listObj))
    {
        return NULL;
    }

    PyObject* valueX = PyList_GetItem(listObj, 0);
    double x = PyFloat_AsDouble(valueX);
    PyObject* valueY = PyList_GetItem(listObj, 1);
    double y = PyFloat_AsDouble(valueY);
    PyObject* valueZ = PyList_GetItem(listObj, 2);
    double z = PyFloat_AsDouble(valueZ);

    dvec3 deltaRotation{x, y, z};
    try
    {
        const auto runId = *static_cast<u64*>(PyModule_GetState(self));
        ofsim_events::EventProcessor::getInstance()->changeThrustRotatation(runId, deltaRotation);
    }
    catch (const std::exception& error)
    {
        PyErr_SetString(PyExc_RuntimeError, error.what());
        return nullptr;
    }

    // Py_DecRef(listObj);

    return PyLong_FromLong(0);
}

PyObject* ofsim_python_integration::PythonMachine::get_orbital_data(PyObject* self, PyObject* argsB)
{
    auto& events = *ofsim_events::EventProcessor::getInstance();
    const auto data = events.getTelemetry();
    return Py_BuildValue(
        "{s:d,s:d,s:d,s:d,s:d,s:d,s:d,s:d,s:d,s:d,s:d,s:d,s:O}", "positionX", data.position.x,
        "positionY", data.position.y, "positionZ", data.position.z, "velocityX", data.velocity.x,
        "velocityY", data.velocity.y, "velocityZ", data.velocity.z, "rotationX", data.rotation.x,
        "rotationY", data.rotation.y, "rotationZ", data.rotation.z, "altitude", data.altitude,
        "thrustMagnitude", data.thrustMagnitude, "rocketMass", data.mass, "shouldStop",
        events.isPythonMachineTerminated() ? Py_True : Py_False);
}

void ofsim_python_integration::PythonMachine::runPythonOrbitalProgram(std::string sourceCode,
                                                                      u64 runId)
{
    // Register once; the interpreter is initialized/finalized for each run.
    static const int registered = PyImport_AppendInittab("orbital_fs", &PyInit_orbital_fs);
    if (registered != 0)
        throw std::runtime_error("Unable to register orbital_fs");
    CPyInstance hInstance;

    // Bind the identifier to this interpreter's module, including Python-created threads.
    PyObject* module = PyImport_ImportModule("orbital_fs");
    if (module == nullptr)
        throw std::runtime_error("Unable to initialize orbital_fs");
    *static_cast<u64*>(PyModule_GetState(module)) = runId;
    Py_DECREF(module);

    PyObject* globals = PyDict_New();
    if (globals == nullptr)
        throw std::runtime_error("Unable to create Python globals");
    PyObject* result = PyRun_String(sourceCode.c_str(), Py_file_input, globals, globals);
    Py_DECREF(globals);
    if (result == NULL)
    {
        PyObject *type, *value, *traceback;
        PyErr_Fetch(&type, &value, &traceback);

        PyObject* str_exc = value ? PyObject_Str(value) : nullptr;
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
