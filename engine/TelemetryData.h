#pragma once

#include <glm/glm.hpp>

class TelemetryData
{
public:
	unsigned __int64 time;

	double altitude;
	double mass;
	double velocity;
	double acceleration;
	double atmPressure;
	double apogeum = 0;
	double perygeum = 0;
	glm::dvec3 position;	
};
