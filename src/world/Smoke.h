#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "../renderer/ObjectRenderer.h"
#include "../math_and_physics/Sphere.h"

namespace ofsim_world
{
    // TODO: use instancing
    struct Smoke
    {    
        Smoke();
        void puff(glm::dmat4& projection, glm::dmat4& view, glm::dvec3& _lightPos, glm::dvec3 position);        

    private:
        int puffIndex{ 0 };
        std::vector<std::unique_ptr<ofsim_math_and_physics::Sphere>> puffClouds;
        std::unique_ptr<ObjectRenderer> renderer;
    };
}
