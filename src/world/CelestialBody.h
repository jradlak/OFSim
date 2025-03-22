#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>
#include <memory>

#include "../math/Sphere.h"
#include "../renderer/ObjectRenderer.h"

namespace ofsim_world
{
    enum CelestialBodyType { star, planet, moon };

    struct CelestialBody
    {   
        CelestialBody(CelestialBodyType _bodyType, std::string shaderName, double _size, glm::dvec3 _position, bool _textured = false);

        void init() { objectRenderer->init(sphere->verticesAndNormals, sphere->indices); }
        void init(glm::dvec3 _objectColor, double _gravity, std::string textureFile);
        void init(glm::dvec3 _objectColor, double _gravity);

        void render(glm::dmat4& projection, dmat4& view, const dvec3& _lightPos, const dvec3& rotation);

        glm::dvec3 pointAboveTheSurface(double theta, double fi, double distance);

    private:
        CelestialBodyType bodyType;
        std::unique_ptr<ofsim_math_and_physics::Sphere> sphere;
        std::unique_ptr<ObjectRenderer> objectRenderer;

        glm::dvec3 position;
        double diameter;

        glm::dvec3 objectColor;

        double gravity;
        bool textured;
    };

}
