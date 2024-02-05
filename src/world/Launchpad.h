#pragma once

#include <glm/glm.hpp>
#include <string>
#include <memory>

#include "../renderer/ModelRenderer.h"

class Launchpad
{
public:
	Launchpad(
		std::string shaderName, 
		std::string modelName,
		glm::dvec3 _position,
		double _size);

	void render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 _lightPos);

	void updateColor(float _r, float _g, float _b);

	void updateRotation(glm::dvec3 newRotation) { rotation = newRotation; }

	void updatePosition(glm::dvec3 newPosiotion) { position = newPosiotion; }

	~Launchpad() {}

private:
	std::unique_ptr<ofsim_renderer::ModelRenderer> modelRenderer;

	glm::dvec3 position;	
	glm::dvec3 rotation;

	//model color;
	float r, g, b;

	double size;
};