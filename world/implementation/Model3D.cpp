#include "..\Model3D.h"

Model3D::Model3D(
	std::string shaderName,
	std::string modelName,
	glm::dvec3 _position, 
	double _size)
	: position(_position), size(_size)
{
	modelRenderer = new ModelRenderer(shaderName, modelName);
	r = 0.25f; g = 0.25f, b = 0.75f;
}

void Model3D::render(glm::dmat4& projection, glm::dmat4& view, glm::dvec3& _lightPos)
{
	Shader* shader = modelRenderer->getShader();
	shader->use();
	shader->setVec3("objectColor", r, g, b);
	shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shader->setVec3("lightPos", _lightPos);

	modelRenderer->renderWithRotation(projection, view, size, position, rotation);
}

void Model3D::updateColor(float _r, float _g, float _b)
{
	r = _r;
	g = _g;
	b = _b;
}

void Model3D::updateRotation(glm::dvec3 newRotation)
{
	rotation = newRotation;
}

void Model3D::updatePosition(glm::dvec3 newPosition)
{
	position = newPosition;
}

Model3D::~Model3D()
{
	delete modelRenderer;
}


