#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../math_and_physics/MathTypes.h"

#include <vector>
#include "Shader.h"

namespace ofsim_renderer
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
    };

    class Mesh
    {
    public:
        // mesh data
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

        void draw(Shader& shader);
    private:
        //  render data
        u32 VAO, VBO, EBO;

        void setupMesh();
    };
}