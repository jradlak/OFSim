#pragma once

#include "MathTypes.h"

namespace ofsim_math_and_physics
{
	struct Geometry 
	{			
		static dvec3 rotateVector(dvec3 v, dvec3 k, f64 angle);
	};
};