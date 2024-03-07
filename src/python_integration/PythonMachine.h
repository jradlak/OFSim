#pragma once

#include <Python.h>
#include <string>
#include <iostream>
#include "../world/Rocket.h"
#include "../infrastructure/EventProcessor.h"

namespace ofsim_python_integration
{
    struct CPyInstance
    {
        CPyInstance()
        {
            Py_Initialize();
        }

        ~CPyInstance()
        {
            Py_Finalize();
        }
    };

    struct PythonMachine
    {                                     
        static PyObject *orbital_thrust_change(PyObject *self, PyObject *args)
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

        static PyObject *orbital_thrust_get(PyObject *self, PyObject *args)
        {
            u64 thrust = ofsim_events::EventProcessor::getInstance()->getRocketValue();
            return PyFloat_FromDouble(thrust);
        }

        void runPythonOrbitalProgram(std::string sourceCode);
    };    

    static struct PyMethodDef methods[] = {
        {"orbital_thrust_change", PythonMachine::orbital_thrust_change, METH_VARARGS, "Change thrust of the rocket engine."},
        {"orbital_thrust_get", PythonMachine::orbital_thrust_get, METH_VARARGS, "Get thrust of the rocker engine."},
        {NULL, NULL, 0, NULL}};

    static struct PyModuleDef modOrbitalFsDef = {
        PyModuleDef_HEAD_INIT, "orbital_fs", NULL, -1, methods,
        NULL, NULL, NULL, NULL};

    static PyObject *PyInit_orbital_fs(void)
    {
        return PyModule_Create(&modOrbitalFsDef);
    }
}