#pragma once

#include <glm/glm.hpp>

#include "../math/MathTypes.h"

namespace SolarSystemConstants
{
	constexpr f64 GConst = -0.00981;
	constexpr f64 earthSize = 6371.0;
	const dvec3 sunPosition { 0.0, 129600000.0, 129600000.0 };
	const dvec3 lightPos { sunPosition };
	const dvec3 earthPos { 0.0, 0.0, 0.0 };
	const dvec3 moonPos { 384400.0, 0.0, 0.0 };
};