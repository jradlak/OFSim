#include "Sun.h"

Sun::Sun(double _size, const glm::dvec3 _position)
{
	celestialBody = new CelestialBody(CelestialBodyType::star, "light_source", _size, _position);
	celestialBody->init(glm::dvec3(0.25f, 0.75f, 0.55f), 9.81);
}

void Sun::render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 lightPos)
{
	celestialBody->render(projection, view, lightPos);
}

Sun::~Sun()
{
	delete celestialBody;
}
