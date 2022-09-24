#include "..\Clouds.h"

Clouds::Clouds(const int _numberOfClouds, CelestialBody& _earth, double _angle, double _dangle)
    : numberOfClouds(_numberOfClouds), earth(_earth), angle(_angle), dangle(_dangle)
{
    modelRenderer = new ModelRenderer("model3d_shader", "models/Cloud.obj");
    calculatePositionsAndSizes();
}

void Clouds::calculatePositionsAndSizes()
{
    for (int i = 0; i < numberOfClouds; i++)
    {
        float rangle = (rand() % 26 + 4) / 10000.0;
        float rdangle = (rand() % 26 + 4) / 10000.0;
        int min1 = rand() % 2;
        if (min1 == 0) rangle *= -1;
        int min2 = rand() % 2;
        if (min2 == 0) rdangle *= -1;

        glm::dvec3 cloudPos = earth.pointAboveTheSurface(angle + rangle, dangle + rdangle, -0.2);
        float size = (rand() % 20 + 17) / 10000.0;

        cloudsPositions.push_back(cloudPos);
        cloudsSizes.push_back(size);
    }
}


