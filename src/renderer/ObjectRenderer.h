#pragma once

#include "../../external_libraries/glad/glad.h"
#include "../../external_libraries/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

#include "Shader.h"

class ObjectRenderer
{
public:
	ObjectRenderer(std::string shaderName, bool _textured = false);

	void init(std::vector<float> vertices, std::vector<int> indices);

	Shader* getShader();

	void render(glm::dmat4& projection, 
		glm::dmat4& view,
		double size,
		const glm::dvec3& position,
		glm::dvec3 rotationAxis = glm::dvec3(0.0, 0.0, 0.0),
		double rotationAngle = 0.0);

	void renderWithRotation(glm::dmat4& projection,
		glm::dmat4& view,
		double size,
		const glm::dvec3& position,
		glm::dvec3 rotation = glm::dvec3(0.0, 0.0, 0.0));

	void loadTexture(std::string fileName);

	~ObjectRenderer();

private:
	Shader* shader;
	unsigned int VAO, VBO, IBO;
	unsigned int indexCount;

	double logDepthBufFC;
	
	bool textured;
	unsigned int texture1;

	void clear();
};