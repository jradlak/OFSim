#pragma once

#include <vector>
#include "TelemetryData.h"
#include "../math_and_physics/MathTypes.h"

namespace ofsim_simulation
{
    struct TelemetryCollector
    {   
        void registerTelemetry(TelemetryData data);
        void clear();

        std::vector<TelemetryData> telemetryHistory;
        std::vector<f64> velocityHistory;
        std::vector<f64> altitudeHistory;
        std::vector<f64> accelerationHistory;
        std::vector<f64> atmPressureHistory;

        f64 maxVelocity = 0;
        f64 lastVelocity = 0;
        f64 maxAltitude = 0;
        f64 maxAcceleration = 0;
        f64 minAcceleration = 0;
        f64 maxAtmPressure = 0;

    private:
        const u32 history_size { 2048 };
        u64 lastTick = 0;
    };
}
