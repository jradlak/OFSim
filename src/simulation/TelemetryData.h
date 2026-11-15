#pragma once

#include <glm/glm.hpp>

namespace ofsim_simulation
{
class TelemetryData
{
public:
    unsigned long long time; // Simulation time in milliseconds.

    double altitude;
    double mass;
    double velocity;
    double acceleration; // Signed rate of change of speed in km/s^2.
    double atmPressure;
    double apogee = 0;
    double perigee = 0;
    glm::dvec3 position;
};
} // namespace ofsim_simulation
