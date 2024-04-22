#pragma once

#include <memory>
#include "CelestialBody.h"

namespace ofsim_world
{
    class Moon
    {
    public:
        Moon(double _size, const glm::dvec3 _position);
        void render(glm::dmat4 projection, glm::dmat4 view, const glm::dvec3 lightPos) { celestialBody->render(projection, view, lightPos); }

    private:
        std::unique_ptr<CelestialBody> celestialBody;
    };
}
