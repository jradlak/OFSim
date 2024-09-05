#include "VectorIndicator.h"

using namespace ofsim_world;

VectorIndicator::VectorIndicator(std::string shaderName, glm::dvec3 _position, IndicatorType type)
{   
    indicatorColor = type == IndicatorType::VELOCITY ? glm::dvec3(1.0, 0.0, 0.0) : glm::dvec3(0.0, 0.0, 1.0);
    indicator = std::make_unique<Rocket>(shaderName, _position, rocket_initial_size);    
}

void VectorIndicator::renderWithMagnitudeAndDirection(dmat4& projection, dmat4& view, const dvec3& _lightPos, 
    dvec3& position, f64 magnitude, dvec3 direction)
{
    indicator->properties().rotation = direction;
    dvec3 stretch = glm::dvec3(0.15, magnitude, 0.15);    
    indicator->properties().position = position;
    indicator->renderWithColorAndStretch(projection, view, _lightPos, indicatorColor, stretch);
}