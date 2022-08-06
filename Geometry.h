#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Geometry
{
public:
	static glm::quat gRotationBetweenVectors(glm::vec3 start, glm::vec3 dest);	
	static glm::quat gLookAt(glm::vec3 direction, glm::vec3 desiredUp);

	static glm::dvec3 rotateVector(glm::dvec3 v, glm::dvec3 k, double angle);
};