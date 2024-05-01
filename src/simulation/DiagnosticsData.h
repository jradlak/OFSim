#pragma once

#include "../math_and_physics/MathTypes.h"

namespace ofsim_simulation
{  
    struct DiagnosticsData
    {
        dvec3 rocketRotation;
        dvec3 rocketPosition;

        dvec3 thrustVectorDirection;
    };
}
