#include "CloudsAndTrees.h"

using namespace ofsim_world;

CloudsAndTrees::CloudsAndTrees(const i32 _numberOfClouds, CelestialBody& _earth, f64 _angle, f64 _dangle)
    : numberOfClouds(_numberOfClouds), earth(_earth), angle(_angle), dangle(_dangle)
{
    cloudRenderer = std::make_unique<ofsim_renderer::ModelRenderer>("model3d_shader", "assets/models/Cloud.obj");
    treeRenderer = std::make_unique<ofsim_renderer::ModelRenderer>("model3d_shader", "assets/models/Tree.obj");

    calculatePositionsAndSizes();
}

void CloudsAndTrees::render(dmat4 projection, dmat4 view, const dvec3 lightPos)
{
    Shader* cloudShader = cloudRenderer->getShader();
    cloudShader->use();
    cloudShader->setVec3("objectColor", 0.8f, 0.8f, 0.8f);
    cloudShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    cloudShader->setVec3("lightPos", lightPos);

    Shader* treeShader = treeRenderer->getShader();
    treeShader->use();
    treeShader->setVec3("objectColor", 0.2, 0.7, 0.1);
    treeShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    treeShader->setVec3("lightPos", lightPos);

    for (int i = 0; i < numberOfClouds; i++) 
    {                
        cloudRenderer->renderWithRotation(projection, view, dvec3(cloudsSizes[i]), cloudsPositions[i], initialRotation);
        treeRenderer->renderWithRotation(projection, view, dvec3(0.00007), treesPositions[i], initialRotation);
    }
}

void CloudsAndTrees::calculatePositionsAndSizes()
{
    for (int i = 0; i < numberOfClouds; i++)
    {
        f32 rangle = (rand() % 46 + 4) / 30.0;
        f32 rdangle = (rand() % 46 + 4) / 30.0;
        i32 min1 = rand() % 2;
        if (min1 == 0) rangle *= -1;
        i32 min2 = rand() % 2;
        if (min2 == 0) rdangle *= -1;

        dvec3 cloudPoint = earth.pointAboveTheSurface(angle, dangle, 6371 + 0.1);

        dvec3 cloudPos = dvec3(rangle, cloudPoint.y + 1.5, rdangle);
        dvec3 treePos = dvec3(-rangle / 3.0, cloudPoint.y - 0.55, -rdangle / 3.0);

        f32 size = (rand() % 100 + 7) / 100000.0;

        cloudsPositions.push_back(cloudPos);
        treesPositions.push_back(treePos);
        cloudsSizes.push_back(size);
    }
}
