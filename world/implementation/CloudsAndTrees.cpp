#include "..\CloudsAndTrees.h"

CloudsAndTrees::CloudsAndTrees(const int _numberOfClouds, CelestialBody& _earth, double _angle, double _dangle)
    : numberOfClouds(_numberOfClouds), earth(_earth), angle(_angle), dangle(_dangle)
{
    cloudRenderer = new ModelRenderer("model3d_shader", "models/Cloud.obj");
    treeRenderer = new ModelRenderer("model3d_shader", "models/tree.obj");

    calculatePositionsAndSizes();
}

void CloudsAndTrees::render(glm::dmat4& projection, glm::dmat4& view, glm::dvec3& lightPos)
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
        cloudRenderer->renderWithRotation(projection, view, cloudsSizes[i], cloudsPositions[i], initialRotation);
        treeRenderer->renderWithRotation(projection, view, 0.00007, treesPositions[i], initialRotation);
    }
}

void CloudsAndTrees::calculatePositionsAndSizes()
{
    for (int i = 0; i < numberOfClouds; i++)
    {
        float rangle = (rand() % 46 + 4) / 5000.0;
        float rdangle = (rand() % 46 + 4) / 5000.0;
        int min1 = rand() % 2;
        if (min1 == 0) rangle *= -1;
        int min2 = rand() % 2;
        if (min2 == 0) rdangle *= -1;

        glm::dvec3 cloudPos = earth.pointAboveTheSurface(angle + rangle, dangle + rdangle, 0.1);
        glm::dvec3 treePos = earth.pointAboveTheSurface(angle - rangle, dangle - rdangle, -0.2);

        float size = (rand() % 100 + 7) / 100000.0;

        cloudsPositions.push_back(cloudPos);
        treesPositions.push_back(treePos);
        cloudsSizes.push_back(size);
    }
}