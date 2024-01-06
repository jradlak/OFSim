#include "Geometry.h"

// based on: https://github.com/opengl-tutorials/ogl/blob/master/common/quaternion_utils.cpp

glm::quat Geometry::gRotationBetweenVectors(glm::vec3 start, glm::vec3 dest)
{
	start = glm::normalize(start);
	dest = glm::normalize(dest);

	float cosTheta = glm::dot(start, dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f)
	{
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length2(rotationAxis) < 0.01)
		{
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);
		}

		rotationAxis = glm::normalize(rotationAxis);
		return glm::angleAxis(glm::radians(180.0f), rotationAxis);
	}

	rotationAxis = glm::cross(start, dest);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return glm::quat(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);
}

glm::quat Geometry::gLookAt(glm::vec3 direction, glm::vec3 desiredUp)
{
	if (glm::length2(direction) < 0.0001f)
	{
		return glm::quat();
	}
	
	glm::vec3 right = glm::cross(direction, desiredUp);
	desiredUp = glm::cross(right, direction);
	glm::quat rot1 = gRotationBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f), direction);	
	glm::vec3 newUp = rot1 * glm::vec3(0.0f, 1.0f, 0.0f);
	glm::quat rot2 = gRotationBetweenVectors(newUp, desiredUp);
	
	return rot2 * rot1;
}

glm::dvec3 Geometry::rotateVector(glm::dvec3 v, glm::dvec3 k, double fi)
{
	fi = glm::radians(fi);
	return v * cos(fi) + glm::cross(k, v) * sin(fi) + k * glm::dot(k, v) * (1.0 - cos(fi));
}
