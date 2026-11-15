#pragma once

#include <vector>
#include <optional>
#include <cstddef>
#include "TelemetryData.h"
#include "../math/MathTypes.h"

namespace ofsim_simulation
{
struct TelemetryCollector
{
    static constexpr std::size_t plotHistoryCapacity = 2048;
    static constexpr std::size_t flightHistoryCapacity = 2048;

    void registerTelemetry(TelemetryData data);
    void clear();

    std::vector<TelemetryData> telemetryHistory;
    std::vector<f64> velocityHistory;
    std::vector<f64> altitudeHistory;
    std::vector<f64> accelerationHistory;
    std::vector<f64> atmPressureHistory;

    f64 maxVelocity = 0;
    f64 maxAltitude = 0;
    f64 maxAcceleration = 0;
    f64 minAcceleration = 0;
    f64 maxAtmPressure = 0;

private:
    static constexpr u64 sampleIntervalMs = 1000;
    std::optional<u64> lastTick;
    f64 lastVelocity = 0;
    u64 historySamples = 0;
    u64 historyStride = 1;

    void registerFlightHistory(const TelemetryData& data);
};
} // namespace ofsim_simulation
