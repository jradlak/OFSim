#pragma once

#include <vector>

#include "..\renderer\Renderable.h"

class SolarSystem : public Renderable
{
public:
	SolarSystem(glm::dvec3 _lightPos);

	void render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 _lightPos);

	~SolarSystem();

private:
	std::vector<Renderable*> clestialBodies;
	glm::dvec3 lightPos;
};

