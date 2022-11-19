#pragma once

#include <glm/glm.hpp>
#include <vector>

//TODO: extract PuffCloud class
class Sphere
{
public:
    Sphere(); //puff cloud
    Sphere(float _radius, unsigned int _sectorCount, unsigned int _stackCount);
    
    std::vector<float> getVertices();
    std::vector<int> getIndices();

    // below: PuffCloud:
    float getSize();
    void updateSize(float newSize);
    glm::dvec3 getPosition();
    void updatePosition(glm::dvec3 newPosition);

    ~Sphere();

private:
    std::vector<float> verticesAndNormals;
    std::vector<float> texCoords;

    std::vector<int> indices;

    unsigned int sectorCount, stackCount;
    
    float size = 0.0f; // used in puff clouds
    float radius;      // used in other cases
    glm::dvec3 position;
    
    void build();
};
