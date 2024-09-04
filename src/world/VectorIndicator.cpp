#include "VectorIndicator.h"

using namespace ofsim_world;

VectorIndicator::VectorIndicator(std::string shaderName, glm::dvec3 _position, double _size)
{
    indicator = std::make_unique<Rocket>(shaderName, _position, _size);
}
