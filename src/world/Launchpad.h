#pragma once

#include <glm/glm.hpp>
#include <string>
#include <memory>

#include "../renderer/ModelRenderer.h"

namespace ofsim_world
{
    struct Launchpad
    {
        Launchpad(
            std::string shaderName,
            std::string modelName,
            glm::dvec3 _position,
            double _size);

        void render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 _lightPos);

        void updateColor(float _r, float _g, float _b);

        glm::dvec3 position;
        glm::dvec3 rotation;

    private:
        std::unique_ptr<ofsim_renderer::ModelRenderer> modelRenderer;

        //model color;
        float r, g, b;

        double size;
    };
}
