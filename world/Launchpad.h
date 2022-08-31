#pragma once

#include <glm/glm.hpp>
#include <string>

#include "..\renderer\ModelRenderer.h"

class Launchpad
{
public:
	Launchpad(std::string shaderName, glm::dvec3& _position, double _size);

	void render(glm::dmat4& projection, glm::dmat4& view, glm::dvec3& _lightPos);

	void updateRotation(glm::dvec3 newRotation);

	~Launchpad();

private:
	ModelRenderer* modelRenderer;

	glm::dvec3& position;	
	glm::dvec3 rotation;

	double size;
};