#include <vector>

class Sphere
{
public:
    Sphere(float _radius, unsigned int _sectorCount, unsigned int _stackCount);

    std::vector<float> getVertices();
    std::vector<int> getIndices();

    ~Sphere();

private:
    std::vector<float> verticesAndNormals;
    std::vector<int> indices;

    unsigned int sectorCount, stackCount;
    float radius;
    void build();
};
