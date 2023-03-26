#pragma once

#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

#include <vector>
#include "Shader.h"
#include "Model.h"


class ModelRenderer
{
public:
	ModelRenderer(std::string shaderName, std::string modelPath);

	void renderWithRotation(glm::dmat4& projection,
		glm::dmat4& view,
		double size,
		glm::dvec3 position,
		glm::dvec3 rotation = glm::dvec3(0.0, 0.0, 0.0));

	Shader* getShader();

	~ModelRenderer();

private:
	Shader* shader;
	Model* objectModel;

	double logDepthBufFC;	
};