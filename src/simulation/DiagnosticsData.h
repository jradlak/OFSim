#pragma once

#include "../math/MathTypes.h"

namespace ofsim_simulation
{  
    struct DiagnosticsData
    {
        dvec3 rocketRotation, rocketPosition;

        dvec3 cameraPosition;

        dvec3 thrustVectorDirection;

        f64 dTheta, dPhi;
    };
}
