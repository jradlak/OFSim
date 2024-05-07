#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "MathTypes.h"

namespace ofsim_math_and_physics
{   
    constexpr f32 pi_const = 3.14159265358979323f;

    //TODO: extract PuffCloud classB
    struct Sphere
    {            
        Sphere() 
            : radius(0.5f), sectorCount(32), stackCount(16), size(0.0f) { build(); }

        Sphere(f32 _radius, u32 _sectorCount, u32 _stackCount)
            : radius(_radius), sectorCount(_sectorCount), stackCount(_stackCount), size(_radius) { build(); }

        std::vector<f32> verticesAndNormals;
        std::vector<i32> indices;
        std::vector<f32> texCoords;

        f32 size { 0.0f }; // used in puff clouds
        f32 radius;        // used in other cases
        dvec3 position;
    
    private:       
        u32 sectorCount, stackCount;
        
        void build();
    };
}
