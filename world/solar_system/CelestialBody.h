#pragma once

#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>

#include "..\Sphere.h"
#include "..\renderer\ObjectRenderer.h"

enum CelestialBodyType { star, planet, moon };

class CelestialBody 
{
public:
	CelestialBody(CelestialBodyType _bodyType, std::string shaderName, double _size, glm::dvec3 _position, bool _textured = false);
	
	~CelestialBody();

	void init();
	void init(glm::dvec3 _objectColor, double _gravity, std::string textureFile);
	void init(glm::dvec3 _objectColor, double _gravity);

	void render(glm::dmat4& projection, glm::dmat4& view, const glm::dvec3& _lightPos);

	glm::dvec3 pointAboveTheSurface(double theta, double fi, double distance);

private:
	CelestialBodyType bodyType;
	Sphere* sphere;
	
	ObjectRenderer* objectRenderer;

	glm::dvec3 position;
	double diameter;
	
	glm::dvec3 objectColor;
	
	double gravity;
	bool textured;
};