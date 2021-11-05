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
	CelestialBody(CelestialBodyType _bodyType, std::string shaderName,
		float _size, glm::vec3& _position);
	
	~CelestialBody();

	void init();

	void render(glm::mat4& projection, glm::mat4& view, glm::vec3& _lightPos);

	void clear();

private:
	CelestialBodyType bodyType;
	Sphere* sphere;
	Shader* shader;

	glm::vec3& position;

	float size;
	unsigned int VAO, VBO, IBO;
	unsigned int indexCount;
};