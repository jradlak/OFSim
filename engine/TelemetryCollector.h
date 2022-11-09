#pragma once

#include <vector>
#include "TelemetryData.h"

class TelemetryCollector
{
public:
	TelemetryCollector() {}

	void registerTelemetry(TelemetryData data);

	~TelemetryCollector() {}

private:
	std::vector<TelemetryData> telemetryHistory;
};