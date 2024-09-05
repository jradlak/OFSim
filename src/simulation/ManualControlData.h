#pragma once

#include "../math_and_physics/MathTypes.h"

namespace ofsim_simulation
{
    struct ManualControlData
    {
        dvec3 rocketRotation;       
        f64 thrustVectorMagnitude;

        dvec3 velocityVector;
        dvec3 thrustVector;
    };
}