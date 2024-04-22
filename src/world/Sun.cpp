#include "Sun.h"

using namespace ofsim_world;

Sun::Sun(double _size, const glm::dvec3 _position)
{
    celestialBody = std::make_unique<CelestialBody>(CelestialBodyType::star, "light_source", _size, _position);
	celestialBody->init(glm::dvec3(0.25f, 0.75f, 0.55f), 9.81);
}
