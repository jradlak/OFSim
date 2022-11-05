#ifndef SOLAR_SYSTEM_CONSTANTS_H
#define SOLAR_SYSTEM_CONSTANTS_H

#include <glm/glm.hpp>

class SolarSystemConstants
{
public:
	static constexpr glm::dvec3 sunPosition = glm::dvec3(0.0, -3185.0, 149600000.0);
	static constexpr glm::dvec3 lightPos = sunPosition;
	static constexpr glm::dvec3 earthPos = glm::dvec3(0.0, -3186.0, 0.0);
	static constexpr glm::dvec3 moonPos = glm::dvec3(384400.0, -3185.0, 0.0);
};

#endif