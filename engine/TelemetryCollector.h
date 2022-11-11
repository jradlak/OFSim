#pragma once

#include <vector>
#include "TelemetryData.h"

class TelemetryCollector
{
public:
	TelemetryCollector() {}

	void registerTelemetry(TelemetryData data);

	std::vector<double> getVelicityHistory();
	std::vector<double> getAltitudeHistory();

	double getMaxVelocity() { return maxVelocity; }
	double getMaxAltitude() { return maxAltitude; }

	~TelemetryCollector() {}

private:
	std::vector<TelemetryData> telemetryHistory;
	std::vector<double> velocityHistory;
	std::vector<double> altitideHistory;

	double maxVelocity = 0;
	double maxAltitude = 0;
	unsigned __int64 lastTick = 0;
};