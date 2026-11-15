#include "TelemetryCollector.h"

#include <algorithm>

using namespace ofsim_simulation;

void TelemetryCollector::registerTelemetry(TelemetryData data)
{
    if (lastTick && (data.time <= *lastTick || data.time - *lastTick < sampleIntervalMs))
        return;

    data.acceleration =
        lastTick ? (data.velocity - lastVelocity) / ((data.time - *lastTick) / 1000.0) : 0.0;
    lastTick = data.time;
    lastVelocity = data.velocity;
    registerFlightHistory(data);

    velocityHistory.push_back(data.velocity);
    altitudeHistory.push_back(data.altitude);
    atmPressureHistory.push_back(data.atmPressure);
    accelerationHistory.push_back(data.acceleration);

    if (velocityHistory.size() > plotHistoryCapacity)
    {
        velocityHistory.erase(velocityHistory.begin());
        altitudeHistory.erase(altitudeHistory.begin());
        atmPressureHistory.erase(atmPressureHistory.begin());
        accelerationHistory.erase(accelerationHistory.begin());
    }

    maxVelocity = std::max(maxVelocity, data.velocity);
    maxAltitude = std::max(maxAltitude, data.altitude);
    maxAtmPressure = std::max(maxAtmPressure, data.atmPressure);
    maxAcceleration = std::max(maxAcceleration, data.acceleration);
    minAcceleration = std::min(minAcceleration, data.acceleration);
}

void TelemetryCollector::registerFlightHistory(const TelemetryData& data)
{
    // Replace the temporary endpoint until the next retained sample is reached.
    if (historySamples > 0 && (historySamples - 1) % historyStride != 0)
        telemetryHistory.pop_back();
    telemetryHistory.push_back(data);
    ++historySamples;

    if (telemetryHistory.size() > flightHistoryCapacity)
    {
        std::size_t retained = 0;
        for (std::size_t index = 0; index < telemetryHistory.size(); index += 2)
            telemetryHistory[retained++] = telemetryHistory[index];
        telemetryHistory.resize(retained);
        historyStride *= 2;
    }
}

void TelemetryCollector::clear()
{
    telemetryHistory.clear();
    velocityHistory.clear();
    altitudeHistory.clear();
    accelerationHistory.clear();
    atmPressureHistory.clear();

    maxVelocity = 0;
    maxAltitude = 0;
    maxAtmPressure = 0;
    maxAcceleration = 0;
    minAcceleration = 0;
    lastTick.reset();
    lastVelocity = 0;
    historySamples = 0;
    historyStride = 1;
}
