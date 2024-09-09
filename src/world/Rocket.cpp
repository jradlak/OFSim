#include "Rocket.h"

#include "SolarSystemConstants.h"

using namespace ofsim_world;

Rocket::Rocket(std::string shaderName, glm::dvec3 _position, double _size, bool withSmoke)
{	
	rocketProperties.position = _position;
	rocketProperties.initialPosition = _position;
	rocketProperties.rotation = glm::dvec3(0.0);
	rocketProperties.velocity = dvec3(0);

	modelRenderer = std::make_unique<ofsim_renderer::ModelRenderer>(shaderName, "assets/models/large_rocket.obj");
	
	if (withSmoke)
	{
		smoke = std::make_unique<Smoke>();
	}

	rocketProperties.size = _size;
    rocketProperties.mass = rocket_initial_mass;
}

void Rocket::reset(glm::dvec3 _position)
{
	rocketProperties.mass = 10;
	rocketProperties.velocity = dvec3(0.0, 0.0, 0.0);
	rocketProperties.position = _position;
}

void Rocket::render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 _lightPos, const dvec3 &direction)
{		
	renderParallelToVectorWithColorAndStretch(projection, view, _lightPos, direction, glm::vec3(0.55f), glm::dvec3(1.0));
}

void ofsim_world::Rocket::renderParallelToVectorWithColorAndStretch(dmat4 projection, dmat4 view, dvec3 _lightPos,
	const dvec3 &direction, vec3 color, dvec3 stretch)
{
	Shader* shader = modelRenderer->getShader();
	shader->use();
	shader->setVec3("objectColor", color);	
	shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
	shader->setVec3("lightPos", _lightPos);

	modelRenderer->renderParallelToVector(projection, view, 
		rocketProperties.size * stretch, rocketProperties.position, direction);		

	if (smoke != nullptr)
	{		
		glm::dvec3 smokePosition = rocketProperties.position + glm::dvec3(0.0, -0.004, 0.0);  
		
		if (rocketProperties.thrustMagnitude > 0.01 && rocketProperties.mass > 3.0)
		{
			smoke->puff(projection, view, _lightPos, smokePosition);
		}
	}
}
