#pragma once

#include <vector>
#include "TelemetryData.h"

class TelemetryCollector
{
public:
	TelemetryCollector() {}

	void registerTelemetry(TelemetryData data);

	std::vector<double> getVelicityHistory();

	double getMaxVelocity() { return maxVelocity; }

	~TelemetryCollector() {}

private:
	std::vector<TelemetryData> telemetryHistory;
	std::vector<double> velocityHistory;

	double maxVelocity = 0;
	unsigned __int64 lastTick = 0;
};