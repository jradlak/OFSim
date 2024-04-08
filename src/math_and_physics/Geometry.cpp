#include "Geometry.h"

using namespace ofsim_math_and_physics;

dvec3 Geometry::rotateVector(dvec3 v, dvec3 k, f64 fi)
{
	fi = glm::radians(fi);
	return v * cos(fi) + glm::cross(k, v) * sin(fi) + k * glm::dot(k, v) * (1.0 - cos(fi));
}