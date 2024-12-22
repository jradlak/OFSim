#pragma once

#include <glm/glm.hpp>

namespace ofsim_simulation
{
    class TelemetryData
    {
    public:
        unsigned long long time;

        double altitude;
        double mass;
        double velocity;
        double acceleration;
        double atmPressure;
        double apogee = 0;
        double perigee = 0;
        glm::dvec3 position;
    };
}
