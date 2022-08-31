#include "..\Launchpad.h"

Launchpad::Launchpad(std::string shaderName, glm::dvec3& _position, double _size)
	: position(_position), size(_size)
{
	modelRenderer = new ModelRenderer(shaderName, "models/launchpad2.obj");
}

void Launchpad::render(glm::dmat4& projection, glm::dmat4& view, glm::dvec3& _lightPos)
{
	Shader* shader = modelRenderer->getShader();
	shader->use();
	shader->setVec3("objectColor", 0.25f, 0.25f, 0.75f);
	shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shader->setVec3("lightPos", _lightPos);

	modelRenderer->renderWithRotation(projection, view, size, position, rotation);
}

void Launchpad::updateRotation(glm::dvec3 newRotation)
{
	rotation = newRotation;
}

Launchpad::~Launchpad()
{
	delete modelRenderer;
}


