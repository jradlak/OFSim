#include "..\TelemetryCollector.h"

void TelemetryCollector::registerTelemetry(TelemetryData data)
{	
	if (data.time > lastTick) {
		telemetryHistory.push_back(data);
		lastTick = data.time;

		velocityHistory.push_back(data.velocity);
		if (velocityHistory.size() > 512)
		{
			velocityHistory.erase(velocityHistory.begin());
		}

		if (data.velocity > maxVelocity)
		{
			maxVelocity = data.velocity;
		}
	}
}

std::vector<double> TelemetryCollector::getVelicityHistory()
{
	return velocityHistory;
}
