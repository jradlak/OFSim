#pragma once

#include "CelestialBody.h"

class Sun
{
public:

	Sun(double _size, const glm::dvec3 _position);

	void render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 lightPos);

	~Sun();

private:
	CelestialBody* celestialBody;
};