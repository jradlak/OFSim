#pragma once
#include <vector>
#include <memory>

#include "CelestialBody.h"
#include "../renderer/ModelRenderer.h"

namespace ofsim_world
{
    struct CloudsAndTrees
    {
        CloudsAndTrees(const i32 _numberOfClouds, CelestialBody& _earth,
            f64 _angle, f64 _dangle);

        void render(dmat4 projection, dmat4 view, const dvec3 lightPos);

    private:
        const int numberOfClouds;

        std::unique_ptr<ofsim_renderer::ModelRenderer> cloudRenderer;
        std::unique_ptr<ofsim_renderer::ModelRenderer> treeRenderer;

        std::vector<dvec3> cloudsPositions;
        std::vector<dvec3> treesPositions;

        std::vector<f64> cloudsSizes;

        f64 angle;
        f64 dangle;
        dvec3 initialRotation {0, 0, 0};

        CelestialBody& earth;

        void calculatePositionsAndSizes();
    };
}
