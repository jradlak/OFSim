#include "SolarSystem.h"
#include "Planet.h"
#include "Moon.h"
#include "Sun.h"
#include "SolarSystemConstants.h"

SolarSystem::SolarSystem()
{	
	earth = std::make_unique<Planet>(12742, SolarSystemConstants::earthPos);
	moon = std::make_unique<Moon>(1737.0, SolarSystemConstants::moonPos);
	sun = std::make_unique<Sun>(1392700.0, SolarSystemConstants::lightPos);	
}

void SolarSystem::render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 _lightPos)
{
	earth->render(projection, view, _lightPos);
	moon->render(projection, view, _lightPos);
	sun->render(projection, view, _lightPos);
}

void SolarSystem::provideRocketInformationAndInit(double _angle, double _dangle, Rocket* _rocket)
{
	rocket = _rocket;
	initCloudsAndTreesForEarth(_angle, _dangle, rocket->getRotation());
}
