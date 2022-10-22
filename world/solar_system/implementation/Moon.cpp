#include "..\Moon.h"

Moon::Moon(double _size, glm::dvec3& _position)
{
	celestialBody = new CelestialBody(CelestialBodyType::moon, "moon_shader", _size, _position);
	celestialBody->init(glm::dvec3(0.55f, 0.55f, 0.55f), 1.6);
}

void Moon::render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 lightPos)
{
	celestialBody->render(projection, view, lightPos);
}

Moon::~Moon()
{
	delete celestialBody;
}
