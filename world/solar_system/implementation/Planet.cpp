#include "..\Planet.h"

Planet::Planet(double _size, const glm::dvec3& _position)
{
	celestialBody = new CelestialBody(CelestialBodyType::planet, "planet_shader", _size, _position);
	celestialBody->init(glm::dvec3(0.25f, 0.75f, 0.55f), 9.81);	
}

void Planet::render(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos)
{
	celestialBody->render(projection, view, lightPos);
	
	if (cloudsAndTrees != NULL)
	{
		cloudsAndTrees->render(projection, view, lightPos);
	}

	if (launchpad != NULL)
	{
		launchpad->render(projection, view, lightPos);
	}
}

void Planet::init(int number, double _angle, double _dangle, glm::dvec3 _rotation)
{
	cloudsAndTrees = new CloudsAndTrees(12, *this->celestialBody, _angle, _dangle);
	cloudsAndTrees->provideInitialRotation(_rotation);

	glm::dvec3 launchpadPos = pointAboveTheSurface(_angle, _dangle, -0.187);
	this->launchpad = new Launchpad("model3d_shader", "models/launchpad2.obj", launchpadPos, 0.05);
	launchpad->updateRotation(_rotation);
}

glm::dvec3 Planet::pointAboveTheSurface(double theta, double phi, double distance)
{
	return celestialBody->pointAboveTheSurface(theta, phi, distance);
}

Planet::~Planet()
{
	delete celestialBody;
}
