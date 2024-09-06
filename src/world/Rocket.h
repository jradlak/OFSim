#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>

#include "../renderer/ModelRenderer.h"

#include "../math_and_physics/MathTypes.h"

#include "Smoke.h"

namespace ofsim_world
{
    constexpr f32 rocket_initial_size { 0.000013 };
    constexpr f32 rocket_initial_mass { 10.0 };

    struct RocketPhysicalProperties
    {
        dvec3 position;
        dvec3 initialPosition;
        dvec3 towards;
        dvec3 rotation;
        dvec3 velocity;

        f64 size, mass, thrustMagnitude;
    };

    struct Rocket
    {    
        Rocket(std::string shaderName, glm::dvec3 _position, double _size, bool withSmoke = true);

        void render(glm::dmat4 projection, dmat4 view, dvec3 _lightPos);        
        void renderWithColorAndStretch(dmat4 projection, dmat4 view, dvec3 _lightPos, 
            vec3 color, dvec3 stretch);
        void renderParallelToVectorWithColorAndStretch(dmat4 projection, dmat4 view, dvec3 _lightPos,        
            const dvec3& direction, vec3 color, dvec3 stretch);

        void reset(glm::dvec3 _position);

        RocketPhysicalProperties& properties() { return rocketProperties; }

    private:
        // rendering:
        std::unique_ptr<ofsim_renderer::ModelRenderer> modelRenderer;
        std::unique_ptr<Smoke> smoke;

        // orientation and physical properties:
        RocketPhysicalProperties rocketProperties;
    };
}
