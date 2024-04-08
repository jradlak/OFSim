#include "Planet.h"

Planet::Planet(f64 _size, const dvec3& _position)
{
	celestialBody = std::make_unique<CelestialBody>(CelestialBodyType::planet, "planet_textured_shader", _size, _position, true);
	celestialBody->init(glm::dvec3(0.25f, 0.75f, 0.55f), 9.81, "assets/textures/8k_earth_daymap.jpg");	
}

void Planet::init(i32 number, f64 _angle, f64 _dangle, dvec3 _rotation)
{
	cloudsAndTrees = std::make_unique<CloudsAndTrees>(12, *this->celestialBody, _angle, _dangle);
	cloudsAndTrees->provideInitialRotation(_rotation);

	dvec3 launchpadPos = pointAboveTheSurface(_angle, _dangle, 6371 - 0.187);
	
	launchpad = std::make_unique<Launchpad>("model3d_shader", "assets/models/launchpad2.obj", launchpadPos, 0.05);
	launchpad->updateRotation(_rotation);
}

void Planet::render(dmat4 projection, dmat4 view, const dvec3 lightPos)
{
	celestialBody->render(projection, view, lightPos);
	
	if (cloudsAndTrees != nullptr)
	{
		cloudsAndTrees->render(projection, view, lightPos);
	}

	if (launchpad != nullptr)
	{
		launchpad->render(projection, view, lightPos);
	}
}
