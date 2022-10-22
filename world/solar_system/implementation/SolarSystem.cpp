#include "..\SolarSystem.h"
#include "..\Planet.h"
#include "..\Moon.h"
#include "..\Sun.h"


SolarSystem::SolarSystem(glm::dvec3 _lightPos)
{
	lightPos = _lightPos;
	//glm::dvec3 lightPos(0.0, -3185.0, 149600000.0);
	glm::dvec3 earthPos(0.0, -3186.0, 0.0);
	glm::dvec3 moonPos(384400.0, -3185.0, 0.0);

	Renderable* earth = new Planet(6371.0, earthPos);
	Renderable* moon = new Moon(1737.0, moonPos);
	Renderable* sun = new Sun(1392700.0, lightPos);

	clestialBodies.push_back(earth);
	clestialBodies.push_back(moon);
	clestialBodies.push_back(sun);

}

void SolarSystem::render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 _lightPos)
{
	for (int i = 0; i < this->clestialBodies.size(); i++)
	{
		this->clestialBodies[i]->render(projection, view, this->lightPos);
	}
}

SolarSystem::~SolarSystem()
{
	for (int i = 0; i < this->clestialBodies.size(); i++)
	{
		delete this->clestialBodies[i];
	}
}
