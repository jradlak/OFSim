#include "TelemetryCollector.h"

void TelemetryCollector::registerTelemetry(TelemetryData data)
{	
	if (data.time > lastTick) {
		telemetryHistory.push_back(data);
		lastTick = data.time;

		velocityHistory.push_back(data.velocity);
		altitideHistory.push_back(data.altitude);
		atmPressureHistory.push_back(data.atmPressure);

		if (velocityHistory.size() > 2048)
		{
			velocityHistory.erase(velocityHistory.begin());
			altitideHistory.erase(altitideHistory.begin());
			atmPressureHistory.erase(atmPressureHistory.begin());
		}

		if (data.velocity > maxVelocity)
		{
			maxVelocity = data.velocity;
		}

		if (data.altitude > maxAltitude)
		{
			maxAltitude = data.altitude;
		}

		if (data.atmPressure > maxAtmPressure)
		{
			maxAtmPressure = data.atmPressure;
		}

		double dVelocity = data.velocity - lastVelocity;
		acceletationHistory.push_back(dVelocity);
		if (dVelocity > maxAcceleration)
		{
			maxAcceleration = dVelocity;
		}

		if (dVelocity < minAcceleration)
		{
			minAcceleration = dVelocity;
		}

		lastVelocity = data.velocity;
	}
}

void TelemetryCollector::clear()
{	
	telemetryHistory.clear();
	velocityHistory.clear();
	altitideHistory.clear();
	acceletationHistory.clear();
	atmPressureHistory.clear();

	maxVelocity = 0;
	maxAltitude = 0;
	maxAtmPressure = 0;
	maxAcceleration = 0;
	minAcceleration = 0;
	lastTick = 0;
}
