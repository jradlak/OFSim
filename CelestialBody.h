#pragma once

#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>

#include "Sphere.h"
#include "Shader.h"

enum CelestialBodyType { star, planet, moon };

class CelestialBody
{
public:
	CelestialBody(CelestialBodyType _bodyType, std::string shaderName, double _size, glm::dvec3& _position);
	
	~CelestialBody();

	void init();

	void render(glm::dmat4& projection, glm::dmat4& view, glm::dvec3& _lightPos);

	void clear();

private:
	CelestialBodyType bodyType;
	Sphere* sphere;
	Shader* shader;

	glm::dvec3& position;

	double size;
	unsigned int VAO, VBO, IBO;
	unsigned int indexCount;

	double logDepthBufFC;	
};