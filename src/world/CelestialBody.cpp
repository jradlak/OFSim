#include <cmath>
#include "CelestialBody.h"

using namespace ofsim_world;

CelestialBody::CelestialBody(CelestialBodyType _bodyType, std::string shaderName, double _size, glm::dvec3 _position, bool _textured)
	: position(_position), textured(_textured)
{	
	bodyType = _bodyType;
	diameter = _size;
    objectRenderer = std::make_unique<ObjectRenderer>(shaderName, _textured);
	
	if (_bodyType == planet) 
	{
        sphere = std::make_unique<ofsim_math_and_physics::Sphere>(1.0f, 256, 128);
	}
	else
	{
        sphere = std::make_unique<ofsim_math_and_physics::Sphere>(1.0f, 32, 16);
	}	
}

void CelestialBody::init(glm::dvec3 _objectColor, double _gravity)
{
	init();
	objectColor = _objectColor;	
	gravity = _gravity;
}

void CelestialBody::init(glm::dvec3 _objectColor, double _gravity, std::string textureFile)
{
	init(_objectColor, _gravity);
	if (textured)
	{
		objectRenderer->loadTexture(textureFile);
	}
}

void CelestialBody::render(dmat4& projection, dmat4& view, const dvec3& _lightPos, const dvec3& rotation)
{
	Shader* shader = objectRenderer->getShader();
	shader->use();
	
	if (bodyType != star)
	{		
		shader->setVec3("objectColor", objectColor.x, objectColor.y, objectColor.z);		
		shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		shader->setVec3("lightPos", _lightPos);
	}

    objectRenderer->renderWithRotation(projection, view, diameter, position, rotation);
}

glm::dvec3 CelestialBody::pointAboveTheSurface(double theta, double phi, double distance)
{
	// theta - polar angle
	// phi - azimuth angle
	double r = diameter / 2.0 + distance;
	double x = r * cos(glm::radians(theta)) * sin(glm::radians(phi));
	double z = r * sin(glm::radians(theta)) * sin(glm::radians(phi));
	double y = r * cos(glm::radians(phi));

	glm::dvec3 result = position + glm::dvec3(x, y, z);
	return result;
}
