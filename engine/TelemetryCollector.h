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
	std::vector<double> getAccelarationHistory();

	double getMaxVelocity() { return maxVelocity; }
	double getMaxAltitude() { return maxAltitude; }
	double getMaxAcceleration() { return maxAcceleration; }
	double getMinAcceleration() { return minAcceleration; }

	~TelemetryCollector() {}

private:
	std::vector<TelemetryData> telemetryHistory;
	std::vector<double> velocityHistory;
	std::vector<double> altitideHistory;
	std::vector<double> acceletationHistory;

	double maxVelocity = 0;
	double lastVelocity = 0;
	
	double maxAltitude = 0;	
	
	double maxAcceleration = 0;
	double minAcceleration = 0;

	unsigned __int64 lastTick = 0;
};