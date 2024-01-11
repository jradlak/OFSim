#pragma once

#include "MathTypes.h"

namespace ofsim_math_and_physics
{
	struct Geometry 
	{
		static quat gRotationBetweenVectors(dvec3 start, dvec3 dest);	
		static quat gLookAt(dvec3 direction, dvec3 desiredUp);

		static dvec3 rotateVector(dvec3 v, dvec3 k, f64 angle);
	};
};