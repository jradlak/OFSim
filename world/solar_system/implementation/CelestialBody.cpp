#include "..\CelestialBody.h"
#include "Math.h"

CelestialBody::CelestialBody(CelestialBodyType _bodyType, std::string shaderName, double _size, glm::dvec3& _position)
	: position(_position)
{	
	bodyType = _bodyType;
	diameter = _size;
	objectRenderer = new ObjectRenderer(shaderName);
	
	if (_bodyType == planet) 
	{
		sphere = new Sphere(0.5f, 256, 128);
	}
	else
	{
		sphere = new Sphere(0.5f, 32, 16);
	}	
}

void CelestialBody::init()
{
	objectRenderer->init(sphere->getVertices(), sphere->getIndices());
}

void CelestialBody::init(glm::dvec3 _objectColor, double _gravity)
{
	init();
	objectColor = _objectColor;	
	gravity = _gravity;
}

void CelestialBody::render(glm::dmat4& projection, glm::dmat4& view, glm::dvec3& _lightPos)
{
	Shader* shader = objectRenderer->getShader();
	shader->use();
	
	if (bodyType != star)
	{		
		shader->setVec3("objectColor", objectColor.x, objectColor.y, objectColor.z);		
		shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		shader->setVec3("lightPos", _lightPos);
	}

	objectRenderer->render(projection, view, diameter, position);
}

glm::dvec3 CelestialBody::pointAboveTheSurface(double theta, double phi, double distance)
{
	// theta - polar angle
	// phi - azimuth angle
	double r = diameter / 2.0 + distance;
	double x = r * cos(glm::radians(theta)) * sin(glm::radians(phi));
	double z = r * sin(glm::radians(theta)) * sin(glm::radians(phi));
	double y = r * cos(glm::radians(phi));

	return position + glm::dvec3(x, y, z);
}

CelestialBody::~CelestialBody()
{
	delete sphere;
	delete objectRenderer;
}