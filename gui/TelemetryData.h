#pragma once

#include <glm/glm.hpp>

class TelemetryData
{
public:
	double altitude;
	double mass;
	double velocity;
	double atmPressure;
	double apogeum = 0;
	double perygeum = 0;
	glm::dvec3 position;
};
