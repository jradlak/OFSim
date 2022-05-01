#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "ObjectRenderer.h"
#include "TriangleGeometry.h"
#include "ModelRenderer.h"
#include "Smoke.h"

class Rocket
{
public:
	Rocket(std::string shaderName, glm::dvec3& _position, double _size);

	void init();
	
	void render(glm::dmat4& projection, glm::dmat4& view, glm::dvec3& _lightPos);

	glm::dvec3 getPosition();

	void updateMass(double newMass);
	void updatePosition(glm::dvec3 newPosition);
	void updateVelocity(glm::dvec3 newVelocity);
	void updateRotation(glm::dvec3 axis, double angle);
	void updateRotation(glm::dvec3 rotation);
	void updateTowards(glm::dvec3 newTowards);
	
	double getMass();
	glm::dvec3 getVelocity();
	glm::dvec3 getRotation();

	~Rocket();
	
private:	
	ModelRenderer* modelRenderer;
	Smoke* smoke;
	
	glm::dvec3& position;
	glm::dvec3 towards;
	glm::dvec3 rotation;
	glm::dvec3 rotatinAxis;
	double rotationAngle;
	
	double size;

	//physics:
	glm::dvec3 velocity;
	double mass;	
};