#include "SolarSystem.h"
#include "Planet.h"
#include "Moon.h"
#include "Sun.h"
#include "SolarSystemConstants.h"

SolarSystem::SolarSystem()
{	
	this->earth = new Planet(6371.0, SolarSystemConstants::earthPos);
	Renderable* moon = new Moon(1737.0, SolarSystemConstants::moonPos);
	Renderable* sun = new Sun(1392700.0, SolarSystemConstants::lightPos);

	clestialBodies.push_back(this->earth);
	clestialBodies.push_back(moon);
	clestialBodies.push_back(sun);
}

void SolarSystem::render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 _lightPos)
{
	for (int i = 0; i < this->clestialBodies.size(); i++)
	{
		this->clestialBodies[i]->render(projection, view, SolarSystemConstants::lightPos);
	}
}

void SolarSystem::provideRocket(Rocket* _rocket)
{
	rocket = _rocket;
}

void SolarSystem::provideRocketInformationAndInit(double _angle, double _dangle, Rocket* _rocket)
{
	rocket = _rocket;
	initCloudsAndTreesForEarth(_angle, _dangle, rocket->getRotation());
}

void SolarSystem::initCloudsAndTreesForEarth(double _angle, double _dangle, glm::dvec3 _rotation)
{
	this->earth->init(12, _angle, _dangle, _rotation);
}

glm::dvec3 SolarSystem::pointAboveEarthSurface(double theta, double phi, double distance)
{
	return this->earth->pointAboveTheSurface(theta, phi, distance);
}

SolarSystem::~SolarSystem()
{
	for (int i = 0; i < this->clestialBodies.size(); i++)
	{
		delete this->clestialBodies[i];
	}
}
