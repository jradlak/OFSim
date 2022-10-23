#pragma once

#include <vector>

#include "Planet.h"
#include "..\renderer\Renderable.h"
#include "..\Rocket.h"

class SolarSystem : public Renderable
{
public:
	SolarSystem(glm::dvec3 _lightPos);

	void render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 _lightPos);

	void provideRocketInformation(double _angle, double _dangle, Rocket* _rocket);
	
	glm::dvec3 pointAboveEarthSurface(double theta, double phi, double distance);

	~SolarSystem();

private:
	std::vector<Renderable*> clestialBodies;
	Planet* earth;
	Rocket* rocket;
	glm::dvec3 lightPos;	

	void initCloudsAndTreesForEarth(double _angle, double _dangle, glm::dvec3 _rotation);
};

