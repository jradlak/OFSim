#pragma once

#include <vector>
#include "TelemetryData.h"

class TelemetryCollector
{
public:
	TelemetryCollector() {}

	void registerTelemetry(TelemetryData data);
	void clear();

	std::vector<double> getVelicityHistory() { return velocityHistory; }
	std::vector<double> getAltitudeHistory() { return altitideHistory; }
	std::vector<double> getAccelarationHistory() { return acceletationHistory; }
	std::vector<double> getAtmPressureHistory() { return atmPressureHistory; }
	std::vector<TelemetryData> getTelemetryHistory() { return telemetryHistory; }

	double getMaxVelocity() { return maxVelocity; }
	double getMaxAltitude() { return maxAltitude; }
	double getMaxAtmPressure() { return maxAtmPressure; }
	double getMaxAcceleration() { return maxAcceleration; }
	double getMinAcceleration() { return minAcceleration; }

	~TelemetryCollector() {}

private:
	std::vector<TelemetryData> telemetryHistory;
	std::vector<double> velocityHistory;
	std::vector<double> altitideHistory;
	std::vector<double> acceletationHistory;
	std::vector<double> atmPressureHistory;

	double maxVelocity = 0;
	double lastVelocity = 0;
	
	double maxAltitude = 0;	
	
	double maxAcceleration = 0;
	double minAcceleration = 0;

	double maxAtmPressure = 0;

	unsigned long long lastTick = 0;
};