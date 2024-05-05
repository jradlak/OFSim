#include "TelemetryCollector.h"

using namespace ofsim_simulation;

void TelemetryCollector::registerTelemetry(TelemetryData data)
{	
	if (data.time > lastTick) {
		telemetryHistory.push_back(data);
		lastTick = data.time;

		velocityHistory.push_back(data.velocity);
        altitudeHistory.push_back(data.altitude);
		atmPressureHistory.push_back(data.atmPressure);

        if (velocityHistory.size() > history_size)
		{
			velocityHistory.erase(velocityHistory.begin());
            altitudeHistory.erase(altitudeHistory.begin());
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
        accelerationHistory.push_back(dVelocity);
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
    altitudeHistory.clear();
    accelerationHistory.clear();
	atmPressureHistory.clear();

	maxVelocity = 0;
	maxAltitude = 0;
	maxAtmPressure = 0;
	maxAcceleration = 0;
	minAcceleration = 0;
	lastTick = 0;
}
