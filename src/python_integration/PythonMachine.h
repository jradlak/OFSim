#pragma once

#include <Python.h>
#include <string>
#include <iostream>

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

    class CPyObject
    {
    private:
        PyObject *p;

    public:
        CPyObject() : p(nullptr)
        {
        }

        CPyObject(PyObject *_p) : p(_p)
        {
        }

        ~CPyObject()
        {
            Release();
        }

        PyObject *getObject()
        {
            return p;
        }

        PyObject *setObject(PyObject *_p)
        {
            return (p = _p);
        }

        PyObject *AddRef()
        {
            if (p)
            {
                Py_INCREF(p);
            }

            return p;
        }

        void Release()
        {
            if (p)
            {
                Py_DECREF(p);
            }

            p = nullptr;
        }

        PyObject *operator->()
        {
            return p;
        }

        bool is()
        {
            return p ? true : false;
        }

        operator PyObject *()
        {
            return p;
        }

        PyObject *operator=(PyObject *pp)
        {
            p = pp;
            return p;
        }

        operator bool()
        {
            return p ? true : false;
        }
    };

    static PyObject* orbital_thrust_change(PyObject* self, PyObject* args)
    {
        PyObject *a;
        if (PyArg_UnpackTuple(args, "", 1, 1, &a))
        {
            std::cout << "orbital_thrust_change: " << PyFloat_AsDouble(a) << std::endl;
        }

        return PyLong_FromLong(0);
    }

    static PyObject* orbital_thrust_get(PyObject* self, PyObject* args)
    {   
        double thrust = 123.0;        
        return PyFloat_FromDouble(thrust);
    }

    static struct PyMethodDef methods[] = {
        { "orbital_thrust_change", orbital_thrust_change, METH_VARARGS, "Change thrust of the rocket engine."},
        { "orbital_thrust_get", orbital_thrust_get, METH_VARARGS, "Get thrust of the rocker engine." },
        { NULL, NULL, 0, NULL }
    };

    static struct PyModuleDef modOrbitalFsDef = {
        PyModuleDef_HEAD_INIT, "orbital_fs", NULL, -1, methods, 
        NULL, NULL, NULL, NULL
    };

    static PyObject* PyInit_orbital_fs(void)
    {
        return PyModule_Create(&modOrbitalFsDef);
    }

    struct PythonMachine
    {
        static void runPythonOrbitalProgram(std::string sourceCode);
    };
}