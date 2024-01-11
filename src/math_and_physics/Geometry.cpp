#include "Geometry.h"

// based on: https://github.com/opengl-tutorials/ogl/blob/master/common/quaternion_utils.cpp

using namespace ofsim_math_and_physics;

quat Geometry::gRotationBetweenVectors(dvec3 start, dvec3 dest)
{
	start = glm::normalize(start);
	dest = glm::normalize(dest);

	float cosTheta = glm::dot(start, dest);
	dvec3 rotationAxis;

	if (cosTheta < -1 + 0.001f)
	{
		rotationAxis = glm::cross(dvec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length2(rotationAxis) < 0.01)
		{
			rotationAxis = glm::cross(dvec3(1.0f, 0.0f, 0.0f), start);
		}

		rotationAxis = glm::normalize(rotationAxis);
		return glm::angleAxis(glm::radians(180.0), rotationAxis);
	}

	rotationAxis = glm::cross(start, dest);

	f64 s = sqrt((1 + cosTheta) * 2);
	f64 invs = 1 / s;

	return glm::quat(
		s * 0.5,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);
}

quat Geometry::gLookAt(dvec3 direction, dvec3 desiredUp)
{
	if (glm::length2(direction) < 0.0001)
	{
		return glm::quat();
	}
	
	dvec3 right = glm::cross(direction, desiredUp);
	desiredUp = glm::cross(right, direction);
	quat rot1 = gRotationBetweenVectors(dvec3(0.0, 0.0, 1.0), direction);	
	dvec3 newUp = rot1 * vec3(0.0f, 1.0f, 0.0f);
	quat rot2 = gRotationBetweenVectors(newUp, desiredUp);
	
	return rot2 * rot1;
}

dvec3 Geometry::rotateVector(dvec3 v, dvec3 k, f64 fi)
{
	fi = glm::radians(fi);
	return v * cos(fi) + glm::cross(k, v) * sin(fi) + k * glm::dot(k, v) * (1.0 - cos(fi));
}