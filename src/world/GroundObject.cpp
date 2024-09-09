#include "GroundObject.h"

using namespace ofsim_world;

GroundObject::GroundObject(
	std::string shaderName,
	std::string modelName,
	glm::dvec3 _position, 
	double _size,
	GroundObjectType type)
		: position(_position), size(_size)
{
	modelRenderer = std::make_unique<ofsim_renderer::ModelRenderer>(shaderName, modelName);
	if (type == GroundObjectType::LAUNCHPAD) 
	{	
		r = 0.15f; g = 0.15f, b = 0.45f;
	}
	else
	{
		r = 0.5f; g = 0.5f; b = 0.5f;		
	}	
}

void GroundObject::render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 _lightPos)
{
	Shader* shader = modelRenderer->getShader();
	shader->use();
	shader->setVec3("objectColor", r, g, b);
	shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shader->setVec3("lightPos", _lightPos);

	modelRenderer->renderWithRotation(projection, view, dvec3(size), position, rotation);
}

void GroundObject::updateColor(float _r, float _g, float _b)
{
	r = _r;
	g = _g;
	b = _b;
}
