#include "Rocket.h"

Rocket::Rocket(std::string shaderName, glm::dvec3& _position, double _size)
	: position(_position), size(_size)
{	
	modelRenderer = new ModelRenderer(shaderName, "12216_rocket_v1_l2.obj");
	
	smoke = new Smoke();

	mass = 10.0;

	rotatinAxis = glm::dvec3(0.0);
	rotationAngle = 0.0;
	rotation = glm::dvec3(0.0);
}

void Rocket::init()
{
	
}

void Rocket::render(glm::dmat4& projection, glm::dmat4& view, glm::dvec3& _lightPos)
{
	//TODO: make it cleaner!!!	
	Shader* shader = modelRenderer->getShader();
	shader->use();
	shader->setVec3("objectColor", 0.55f, 0.55f, 0.55f);
	shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shader->setVec3("lightPos", _lightPos);

	modelRenderer->renderWithRotation(projection, view, size, position, rotation);

	glm::dvec3 direction = glm::normalize(position - towards);
	glm::dvec3 smokePosition = position - (direction / 100.0);
	
	//TODO: needs better solution
	if (mass > 3.0)
	{
		smoke->puff(projection, view, _lightPos, smokePosition);
	}
}

glm::dvec3 Rocket::getPosition()
{
	return position;
}

double Rocket::getMass()
{
	return mass;
}

glm::dvec3 Rocket::getVelocity()
{
	return velocity;
}

glm::dvec3 Rocket::getRotation()
{
	return rotation;
}

void Rocket::updateMass(double newMass)
{
	mass = newMass;
}

void Rocket::updatePosition(glm::dvec3 newPosition)
{
	position = newPosition;
}

void Rocket::updateVelocity(glm::dvec3 newVelocity)
{
	velocity = newVelocity;
}

void Rocket::updateRotation(glm::dvec3 axis, double angle)
{
	rotatinAxis = axis;
	rotationAngle = angle;
}

void Rocket::updateRotation(glm::dvec3 _rotation)
{
	rotation = _rotation;
}

void Rocket::updateTowards(glm::dvec3 newTowards)
{
	towards = newTowards;
}

Rocket::~Rocket()
{
	delete modelRenderer;
	delete smoke;
}