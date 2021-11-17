#pragma once

#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

#include "Shader.h"

class ObjectRenderer
{
public:
	ObjectRenderer(std::string shaderName);

	void init(std::vector<float> vertices, std::vector<int> indices);

	Shader* getShader();

	void render(glm::dmat4& projection, 
		glm::dmat4& view,
		double size,
		glm::dvec3 position,
		glm::dvec3 rotation = glm::dvec3(0.0, 0.0, 0.0));

	void clear();

	~ObjectRenderer();

private:
	Shader* shader;
	unsigned int VAO, VBO, IBO;
	unsigned int indexCount;

	double logDepthBufFC;
};