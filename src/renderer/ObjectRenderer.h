#pragma once

#include <glad/glad.h>
#include "../../external_libraries/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <memory>

#include "Shader.h"

struct ObjectRenderer
{
	ObjectRenderer(std::string shaderName, bool _textured = false);

	void init(std::vector<float> vertices, std::vector<int> indices);


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

    // TODO: think about how to solve it better!
    Shader* getShader() { return shader.get(); }

    ~ObjectRenderer();

private:
    std::unique_ptr<Shader> shader;
	unsigned int VAO, VBO, IBO;
	unsigned int indexCount;

	double logDepthBufFC;
	
	bool textured;
	unsigned int texture1;

	void clear();
};
