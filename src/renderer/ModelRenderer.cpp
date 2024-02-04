#include "ModelRenderer.h"

using namespace ofsim_renderer;	

ModelRenderer::ModelRenderer(std::string shaderName, std::string modelPath)
{
	shader = std::make_unique<Shader>(("shaders/" + shaderName + "_vs.glsl").c_str(), ("shaders/" + shaderName + "_fs.glsl").c_str());
	objectModel = std::make_unique<Model>(modelPath);
	logDepthBufFC = 2.0 / log(150000000000000000.0 + 1.0) / 0.69315;
}

void ModelRenderer::renderWithRotation(dmat4& projection, dmat4& view, f64 size, dvec3 position, dvec3 rotation)
{
	shader->use();

	shader->setFloat("logDepthBufFC", logDepthBufFC);

	glm::dmat4 model = glm::dmat4(1.0);
	
	model = glm::translate(model, position);

	// calculate rotations:	
		
	if (rotation.x != 0.0)
	{
		model = glm::rotate(model, glm::radians(rotation.x), dvec3(1.0, 0.0, 0.0));
	}

	if (rotation.y != 0.0)
	{
		model = glm::rotate(model, glm::radians(rotation.y), dvec3(0.0, 1.0, 0.0));
	}

	if (rotation.z != 0.0)
	{
		model = glm::rotate(model, glm::radians(rotation.z), dvec3(0.0, 0.0, 1.0));
	}

	//model = glm::rotate(model, glm::radians(20.0), rotation);

	model = glm::scale(model, dvec3(size));
	shader->setMat4("model", mat4(model));

	glm::mat4 transformation = mat4((projection * view * model));
	shader->setMat4("transformation", transformation);

	objectModel->draw(*shader);
}

