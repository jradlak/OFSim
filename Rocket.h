#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include "ObjectRenderer.h"
#include "TriangleGeometry.h"

class Rocket
{
public:
	Rocket(std::string shaderName, glm::dvec3& _position, double _size, 
		glm::dvec3 _rotation = glm::dvec3(0.0, 0.0, 0.0));

	void init();
	
	void render(glm::dmat4& projection, glm::dmat4& view, glm::dvec3& _lightPos);

	glm::dvec3 getPosition();

	//physics:
	void updatePhysics(double deltaTime);
	void addForce(glm::vec3 force);
	void resetForces();

	void updatePosition(glm::dvec3 newPosition);
	void updateRotation(glm::dvec3 newRotation);

	~Rocket();

private:
	ObjectRenderer* objectRenderer;
	TriangleGeometry* geometry;
	glm::dvec3& position;
	glm::dvec3& rotation;
	double size;


	//physics:
	std::vector<glm::dvec3> forces;
	glm::dvec3 velocity;
	double mass;

	//helper methods: 
	void makeRocketGeometry();

	void addTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	glm::vec3 point(double x, double y, double z);
};