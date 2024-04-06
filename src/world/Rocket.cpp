#include "Rocket.h"

Rocket::Rocket(std::string shaderName, glm::dvec3 _position, double _size)
{	
	rocketProperties.position = _position;
	rocketProperties.initialPosition = _position;
	rocketProperties.rotation = glm::dvec3(0.0);
	rocketProperties.velocity = dvec3(0);

	modelRenderer = std::make_unique<ofsim_renderer::ModelRenderer>(shaderName, "assets/models/12216_rocket_v1_l2.obj");
	
	smoke = std::make_unique<Smoke>();

	rocketProperties.size = _size;
	rocketProperties.mass = 10.0;
}

void Rocket::reset(glm::dvec3 _position)
{
	rocketProperties.mass = 10;
	rocketProperties.velocity = dvec3(0.0, 0.0, 0.0);
	rocketProperties.position = _position;
}

void Rocket::render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 _lightPos)
{
	//TODO: make it cleaner!!!	
	Shader* shader = modelRenderer->getShader();
	shader->use();
	shader->setVec3("objectColor", 0.55f, 0.55f, 0.55f);
	shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shader->setVec3("lightPos", _lightPos);

	modelRenderer->renderWithRotation(projection, view, rocketProperties.size, rocketProperties.position, rocketProperties.rotation);
	 
	glm::dvec3 direction = glm::normalize(rocketProperties.position - rocketProperties.towards);
	glm::dvec3 smokePosition = rocketProperties.position - (direction / 100.0) + glm::dvec3(0.004, 0.0015, 0.0);
	 
	if (rocketProperties.thrustMagnitude > 0.01 && rocketProperties.mass > 3.0)
	{
		smoke->puff(projection, view, _lightPos, smokePosition);
	}
}