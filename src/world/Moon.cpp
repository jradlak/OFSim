#include "Moon.h"

using namespace ofsim_world;

Moon::Moon(double _size, const glm::dvec3 _position)
{
    celestialBody = std::make_unique<CelestialBody>(CelestialBodyType::moon, "moon_shader", _size, _position);
	celestialBody->init(glm::dvec3(0.55f, 0.55f, 0.55f), 1.6);
}
