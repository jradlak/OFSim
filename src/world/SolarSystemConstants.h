#pragma once
#include <glm/glm.hpp>

namespace SolarSystemConstants
{
	const glm::dvec3 sunPosition { glm::dvec3(0.0, -3185.0, 149600000.0) };
	const glm::dvec3 lightPos { sunPosition };
	const glm::dvec3 earthPos { glm::dvec3(0.0, -3186.0, 0.0) };
	const glm::dvec3 moonPos { glm::dvec3(384400.0, -3185.0, 0.0) };
};