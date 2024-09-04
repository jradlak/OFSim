#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <string>

#include "Rocket.h"

namespace ofsim_world
{
    class VectorIndicator
    {
    public:  
        VectorIndicator(std::string shaderName, glm::dvec3 _position, double _size);

    private:
        std::unique_ptr<Rocket> indicator;
    };
}
