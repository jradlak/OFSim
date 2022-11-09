#include "..\TelemetryCollector.h"

void TelemetryCollector::registerTelemetry(TelemetryData data)
{
	telemetryHistory.push_back(data);
}
