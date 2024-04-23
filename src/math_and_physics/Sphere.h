#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "MathTypes.h"

namespace ofsim_math_and_physics
{   
    constexpr f32 M_PI{ 3.14159265358979323 };

    //TODO: extract PuffCloud classB
    class Sphere
    {        
    public:
        Sphere() 
            : radius(0.5f), sectorCount(32), stackCount(16), size(0.0f) { build(); }

        Sphere(f32 _radius, u32 _sectorCount, u32 _stackCount)
            : radius(_radius), sectorCount(_sectorCount), stackCount(_stackCount), size(_radius) { build(); }
        
        std::vector<f32> getVertices() { return verticesAndNormals; }
        std::vector<i32> getIndices() { return indices; }

        // below: PuffCloud:
        f32 getSize() { return size; }
        void updateSize(f32 newSize) { size = newSize; }
        dvec3 getPosition() { return position; }
        void updatePosition(glm::dvec3 newPosition) { position = newPosition; }
    
    private:
        std::vector<f32> verticesAndNormals;
        std::vector<f32> texCoords;

        std::vector<i32> indices;

        u32 sectorCount, stackCount;
        
        f32 size { 0.0f }; // used in puff clouds
        f32 radius;        // used in other cases
        dvec3 position;
        
        void build();
    };
}