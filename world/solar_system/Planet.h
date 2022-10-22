#pragma once

#include "..\renderer\Renderable.h"
#include "CelestialBody.h"

class Planet : public Renderable 
{
public:
	Planet(double _size, glm::dvec3& _position);

	void render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 lightPos);
	
	glm::dvec3 pointAboveTheSurface(double theta, double phi, double distance);

	~Planet();
private:
	CelestialBody* celestialBody;	
};