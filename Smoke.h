#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "ObjectRenderer.h"
#include "Sphere.h"

// TODO: use instancing
class Smoke
{
public:
	Smoke();
	void puff(glm::dmat4& projection, glm::dmat4& view, glm::dvec3& _lightPos, glm::dvec3 position);
	~Smoke();

private:
	int puffIndex = 0;
	std::vector<Sphere*> puffClouds;
	ObjectRenderer* renderer;
};