#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
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

    class Rocket
    {
    public:
        Rocket(std::string shaderName, glm::dvec3 _position, double _size);

        void render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 _lightPos);

        RocketPhysicalProperties& projectProperties() { return rocketProperties; }

        void reset(glm::dvec3 _position);

        f64 getMass() { return rocketProperties.mass; }
        glm::dvec3& getVelocity() { return rocketProperties.velocity; }
        glm::dvec3& getPosition() { return rocketProperties.position; }
        glm::dvec3& getRotation() {	return rocketProperties.rotation; }

    private:
        // rendering:
        std::unique_ptr<ofsim_renderer::ModelRenderer> modelRenderer;
        std::unique_ptr<Smoke> smoke;

        // orientation and physical properties:
        RocketPhysicalProperties rocketProperties;
    };
}
