#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Geometry
{
public:
	static glm::quat gRotationBetweenVectors(glm::vec3 start, glm::vec3 dest);	
	static glm::quat gLookAt(glm::vec3 direction, glm::vec3 desiredUp);
};