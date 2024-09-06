#include "ModelRenderer.h"

#include<glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include<glm/common.hpp>

using namespace ofsim_renderer;	

ModelRenderer::ModelRenderer(std::string shaderName, std::string modelPath)
{
	shader = std::make_unique<Shader>(("shaders/" + shaderName + "_vs.glsl").c_str(), ("shaders/" + shaderName + "_fs.glsl").c_str());
	objectModel = std::make_unique<Model>(modelPath);
	logDepthBufFC = 2.0 / log(150000000000000000.0 + 1.0) / 0.69315;
}

void ModelRenderer::renderWithRotation(dmat4& projection, dmat4& view, dvec3 size, dvec3 position, dvec3 rotation)
{
	shader->use();

	shader->setFloat("logDepthBufFC", logDepthBufFC);

	glm::dmat4 model = glm::dmat4(1.0);
	
	// calculate rotations:			
	model = glm::translate(model, position);
	model = glm::scale(model, size);

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
			
	shader->setMat4("model", mat4(model));

	glm::mat4 transformation = mat4((projection * view * model));
	shader->setMat4("transformation", transformation);

	objectModel->draw(*shader);
}

void ModelRenderer::renderParallelToVector(dmat4 &projection, dmat4 &view, dvec3 size, 
	const dvec3& position, const dvec3& direction)
{
	shader->use();

	shader->setFloat("logDepthBufFC", logDepthBufFC);

	glm::dmat4 model = glm::dmat4(1.0);
	
	// calculate rotations:			
	model = glm::translate(model, position);	
	model = glm::scale(model, size);
	model = model * alignToDirection(direction);
			
	shader->setMat4("model", mat4(model));

	glm::mat4 transformation = mat4((projection * view * model));
	shader->setMat4("transformation", transformation);

	objectModel->draw(*shader);
}

dmat4 ModelRenderer::alignToDirection(const dvec3& direction)
{
	dvec3 targetVector = glm::normalize(direction);

    dvec3 defaultOrientation(0.0, 1.0, 0.0);
    
    dvec3 rotationAxis = glm::cross(defaultOrientation, targetVector);

    f64 cosTheta = glm::dot(defaultOrientation, targetVector);
    f64 angle = acos(cosTheta);

    glm::dquat quaternion = glm::angleAxis(angle, rotationAxis);
    glm::dmat4 rotationMatrix = glm::toMat4(quaternion);

    return rotationMatrix;
}
