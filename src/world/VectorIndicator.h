#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <string>

#include "Rocket.h"

namespace ofsim_world
{   
    enum class IndicatorType
    {
        VELOCITY,
        THRUST_DIRECTION
    };

    class VectorIndicator
    {
    public:  
        VectorIndicator(std::string shaderName, glm::dvec3 _position, IndicatorType type);

        void renderWithMagnitudeAndDirection(dmat4& projection, dmat4& view, const dvec3& _lightPos, 
            dvec3& position, f64 maginitude, dvec3 direction);

    private:
        dvec3 indicatorColor;
        std::unique_ptr<Rocket> indicator;
    };
}
