#include "..\Planet.h"

Planet::Planet(double _size, glm::dvec3& _position)
{
	celestialBody = new CelestialBody(CelestialBodyType::planet, "planet_shader", _size, _position);
	celestialBody->init(glm::dvec3(0.25f, 0.75f, 0.55f), 9.81);
}

void Planet::render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 lightPos)
{
	celestialBody->render(projection, view, lightPos);
}

glm::dvec3 Planet::pointAboveTheSurface(double theta, double phi, double distance)
{
	return celestialBody->pointAboveTheSurface(theta, phi, distance);
}

Planet::~Planet()
{
	delete celestialBody;
}