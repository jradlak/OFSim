#pragma once

#include "CelestialBody.h"

class Moon
{
public:
	Moon(double _size, const glm::dvec3 _position);
	
	void render(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos);

	~Moon();
private:
	CelestialBody* celestialBody;	
};