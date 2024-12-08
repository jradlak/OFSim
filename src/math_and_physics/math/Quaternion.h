#pragma once;

#include "../MathTypes.h";
#include "Vector.h"

// 3D Quaternion implementations.
// Since glm implementation is "experimental" and there is no easy way to crete quaternion instances
// I will write my own implemntation based on the book "Physics for Game Developers" by David M. Bourg

//using Quaternion = quat;

class Quaternion 
{
public:
    f64        n;       // scalar part of the quaternion
    Vector     v;       // vector part

    Quaternion(void) : n(0)
    {
        v.x = 0; v.y = 0; v.z = 0;
    }

    Quaternion(f64 e0, f64 e1, f64 e2, f64 e3) : n(e0)
    {
        v.x = e1; v.y = e2; v.z = e3;
    }

    // TODO: methods and operators:
    //  magnitude
    //  getVector
    //  getScalat
    //  operators: +-*/~
    //  to / from euler angles
    //  etc...
};