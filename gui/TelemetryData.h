#pragma once

#include <glm/glm.hpp>

class TelemetryData
{
public:
	double altitude;
	double mass;
	double velocity;
	double atmPressure;
	glm::dvec3 position;
};
