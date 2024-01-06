#include "ModelRenderer.h"

ModelRenderer::ModelRenderer(std::string shaderName, std::string modelPath)
{
	shader = new Shader(("src/shaders/" + shaderName + "_vs.glsl").c_str(), ("src/shaders/" + shaderName + "_fs.glsl").c_str());
	objectModel = new Model(modelPath);
	logDepthBufFC = 2.0 / log(150000000000000000.0 + 1.0) / 0.69315;
}

void ModelRenderer::renderWithRotation(glm::dmat4& projection, glm::dmat4& view, double size, glm::dvec3 position, glm::dvec3 rotation)
{
	shader->use();

	shader->setFloat("logDepthBufFC", logDepthBufFC);

	glm::dmat4 model = glm::dmat4(1.0);
	
	model = glm::translate(model, position);

	// calculate rotations:	
		
	if (rotation.x != 0.0)
	{
		model = glm::rotate(model, glm::radians(rotation.x), glm::dvec3(1.0, 0.0, 0.0));
	}

	if (rotation.y != 0.0)
	{
		model = glm::rotate(model, glm::radians(rotation.y), glm::dvec3(0.0, 1.0, 0.0));
	}

	if (rotation.z != 0.0)
	{
		model = glm::rotate(model, glm::radians(rotation.z), glm::dvec3(0.0, 0.0, 1.0));
	}

	//model = glm::rotate(model, glm::radians(20.0), rotation);

	model = glm::scale(model, glm::dvec3(size));
	shader->setMat4("model", glm::mat4(model));

	glm::mat4 transformation = glm::mat4((projection * view * model));
	shader->setMat4("transformation", transformation);

	objectModel->draw(*shader);
}

Shader* ModelRenderer::getShader()
{
	return shader;
}

ModelRenderer::~ModelRenderer()
{
	delete objectModel;
	delete shader;
}
