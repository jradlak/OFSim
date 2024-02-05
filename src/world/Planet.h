#pragma once

#include "CloudsAndTrees.h"
#include "CelestialBody.h"
#include "Launchpad.h"

class Planet
{
public:
	Planet(double _size, const glm::dvec3& _position);

	void render(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos);
	
	void init(int number, double _angle, double _dangle, glm::dvec3 _rotation);

	glm::dvec3 pointAboveTheSurface(double theta, double phi, double distance);

	~Planet();

private:
	CelestialBody* celestialBody;	

	CloudsAndTrees* cloudsAndTrees = NULL;

	Launchpad* launchpad = NULL;
};