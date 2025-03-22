#include <gtest/gtest.h>

#include "../src/math/Sphere.h"

using namespace ofsim_math_and_physics;

TEST(SphereTests, testSphere)
{
    Sphere sphere = Sphere(1.0f, 32, 16);

    std::vector<float> vertices = sphere.verticesAndNormals;
    std::vector<int> indices = sphere.indices;

    EXPECT_EQ(vertices.size(), 3366);
    EXPECT_EQ(indices.size(), 2880);
}
