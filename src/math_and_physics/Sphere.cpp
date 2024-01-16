#include <vector>

#include "Sphere.h"

Sphere::Sphere() 
    : radius(0.5f), sectorCount(32), stackCount(16), size(0.0f)
{
    build();
}

Sphere::Sphere(f32 _radius, u32 _sectorCount, u32 _stackCount)
    : radius(_radius), sectorCount(_sectorCount), stackCount(_stackCount), size(_radius)
{
    build();
}

void Sphere::build()
{
    std::vector<f32>().swap(verticesAndNormals);
    std::vector<int>().swap(indices);

    f32 x, y, z, xy;
    f32 nx, ny, nz, lengthInv = 1.0f / radius;
    f32 s, t;

    f32 sectorStep = 2 * M_PI / sectorCount;
    f32 stackStep = M_PI / stackCount;
    f32 sectorAngle, stackAngle;

    for(u32 i = 0; i <= stackCount; ++i)
    {
        stackAngle = M_PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for(u32 j = 0; j <= sectorCount; ++j)
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

            // vertex tex coord (s, t) range between [0, 1]
            s = (f32)j / sectorCount;
            t = (f32)i / stackCount;
            texCoords.push_back(s);
            texCoords.push_back(t);
        }
    }

    u32 k1, k2;
    for (u32 i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (u32 j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}