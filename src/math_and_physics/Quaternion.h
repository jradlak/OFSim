#pragma once

#include "MathTypes.h"

// 3D Quaternion implementations.
// Since glm implementation is "experimental" and there is no easy way to crete quaternion instances
// I will write my own implemntation based on the book "Physics for Game Developers" by David M. Bourg

//using Quaternion = quat;

class Quaternion 
{
public:
    f64       n;          // scalar part of the quaternion
    dvec3     v{0};        // vector part

    Quaternion(void) : n(0) {}
    Quaternion(f64 e0, f64 e1, f64 e2, f64 e3) : n(e0)
    {
        v = dvec3(e1, e2, e3);
    }

    // TODO: methods and operators:
    //  magnitude
    //  getVector
    //  getScalat
    //  operators: +-*/~
    //  to / from euler angles
    //  etc...
};