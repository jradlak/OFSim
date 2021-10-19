#include <vector>

#define M_PI       3.14159265358979323846   // pi

#include "Sphere.h"

Sphere::Sphere(float _radius, unsigned int _sectorCount, unsigned int _stackCount)
{
    radius = _radius;
    sectorCount = _sectorCount;
    stackCount = _stackCount;

    build();
}

void Sphere::build()
{
    std::vector<float>().swap(verticesAndNormals);
    std::vector<int>().swap(indices);

    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / radius;

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;

            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            verticesAndNormals.push_back(x);
            verticesAndNormals.push_back(y);
            verticesAndNormals.push_back(z);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            verticesAndNormals.push_back(nx);
            verticesAndNormals.push_back(ny);
            verticesAndNormals.push_back(nz);
        }
    }

    int k1, k2;
    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (stackCount-1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}

std::vector<float> Sphere::getVertices()
{
    return verticesAndNormals;
}

std::vector<int> Sphere::getIndices()
{
    return indices;
}

Sphere::~Sphere()
{

}
