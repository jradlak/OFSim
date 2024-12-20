#pragma once

// based on: https://learnopengl.com/Model-Loading/Model

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Shader.h"
#include "Mesh.h"

namespace ofsim_renderer
{
    struct Model
    {    
        Model(std::string path) { loadModel(path); }
        void draw(Shader& shader);

    private:
        // model data
        std::vector<Mesh> meshes;
        std::string directory;

        void loadModel(std::string path);
        void processNode(aiNode* node, const aiScene* scene);
        Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    };
}
