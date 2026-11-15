#include "PythonMachine.h"

#include "../infrastructure/EventProcessor.h"

#include <stdexcept>
#include <cmath>

namespace
{
bool finiteValue(double value, const char* message)
{
    if (std::isfinite(value))
        return true;
    PyErr_SetString(PyExc_ValueError, message);
    return false;
}
} // namespace

PyObject* ofsim_python_integration::PythonMachine::orbital_thrust_magnitude_change(PyObject* self,
                                                                                   PyObject* args)
{
    double thrust;
    if (!PyArg_ParseTuple(args, "d:orbital_thrust_magnitude_change", &thrust))
        return nullptr;
    if (!finiteValue(thrust, "Thrust magnitude must be finite"))
        return nullptr;

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

    return PyLong_FromLong(0);
}

PyObject* ofsim_python_integration::PythonMachine::orbital_rocket_rotation_change(PyObject* self,
                                                                                  PyObject* args)
{
    PyObject* rotation;
    if (!PyArg_ParseTuple(args, "O:orbital_rocket_rotation_change", &rotation))
        return nullptr;
    if (!PyList_Check(rotation) && !PyTuple_Check(rotation))
    {
        PyErr_SetString(PyExc_TypeError, "Rotation must be a list or tuple of three numbers");
        return nullptr;
    }

    // Keep elements alive even if a numeric conversion mutates the original list.
    PyObject* values = PySequence_Tuple(rotation);
    if (values == nullptr)
        return nullptr;
    if (PyTuple_Size(values) != 3)
    {
        Py_DECREF(values);
        PyErr_SetString(PyExc_ValueError, "Rotation must contain exactly three numbers");
        return nullptr;
    }

    dvec3 deltaRotation;
    for (int index = 0; index < 3; ++index)
    {
        deltaRotation[index] = PyFloat_AsDouble(PyTuple_GetItem(values, index));
        if (PyErr_Occurred() ||
            !finiteValue(deltaRotation[index], "Rotation values must be finite"))
        {
            Py_DECREF(values);
            return nullptr;
        }
    }
    Py_DECREF(values);

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

    return PyLong_FromLong(0);
}

PyObject* ofsim_python_integration::PythonMachine::get_orbital_data(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args, ":get_orbital_data"))
        return nullptr;

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
        PyErr_NormalizeException(&type, &value, &traceback);

        PyObject* name = type ? PyObject_GetAttrString(type, "__name__") : nullptr;
        const char* type_name = name ? PyUnicode_AsUTF8(name) : nullptr;
        PyObject* str_exc = value ? PyObject_Str(value) : nullptr;
        const char* text = str_exc ? PyUnicode_AsUTF8(str_exc) : nullptr;
        std::string error_message = type_name ? std::string(type_name) + ": " : "";
        error_message += text ? text : "Unknown Python script error";

        Py_XDECREF(name);
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
