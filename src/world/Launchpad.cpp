#include "Launchpad.h"

using namespace ofsim_world;

Launchpad::Launchpad(
	std::string shaderName,
	std::string modelName,
	glm::dvec3 _position, 
	double _size)
	: position(_position), size(_size)
{
	modelRenderer = std::make_unique<ofsim_renderer::ModelRenderer>(shaderName, modelName);
	r = 0.25f; g = 0.25f, b = 0.75f;
}

void Launchpad::render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 _lightPos)
{
	Shader* shader = modelRenderer->getShader();
	shader->use();
	shader->setVec3("objectColor", r, g, b);
	shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shader->setVec3("lightPos", _lightPos);

	modelRenderer->renderWithRotation(projection, view, dvec3(size), position, rotation);
}

void Launchpad::updateColor(float _r, float _g, float _b)
{
	r = _r;
	g = _g;
	b = _b;
}
