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

std::vector<double> TelemetryCollector::getVelicityHistory()
{
	return velocityHistory;
}

std::vector<double> TelemetryCollector::getAltitudeHistory()
{
	return altitideHistory;
}

std::vector<double> TelemetryCollector::getAccelarationHistory()
{
	return acceletationHistory;
}

std::vector<double> TelemetryCollector::getAtmPressureHistory()
{
	return atmPressureHistory;
}

std::vector<TelemetryData> TelemetryCollector::getTelemetryHistory()
{
	return telemetryHistory;
}
