#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <memory>

#include "../renderer/Renderable.h"
#include "../renderer/ObjectRenderer.h"
#include "../renderer/ModelRenderer.h"

#include "../math_and_physics/MathTypes.h"

#include "Smoke.h"

struct RocketPhysicalProperties
{	
	dvec3 &position;
	dvec3 &initialPosition;
	dvec3 &towards;
	dvec3 &rotation;	
	dvec3 &velocity;

	f64 &size, &mass, &thrustMagnitude;	
};

class Rocket : public Renderable
{
public:
	Rocket(std::string shaderName, glm::dvec3 _position, double _size);

	void init() {}
	
	void render(glm::dmat4 projection, glm::dmat4 view, glm::dvec3 _lightPos);

	glm::dvec3 getPosition();

	void updateMass(double newMass);
	void updatePosition(glm::dvec3 newPosition);
	void updateVelocity(glm::dvec3 newVelocity);
	
	void updateRotation(glm::dvec3 rotation);
	void updateTowards(glm::dvec3 newTowards);
	void updateThrustMagnitude(double newMagnitude) { thrustMagnitude = newMagnitude; }

	RocketPhysicalProperties projectProperties();

	void reset();

	f64 getMass();
	glm::dvec3 getVelocity();
	glm::dvec3 getRotation();

	~Rocket() {}
	
private:	
	// rendering:
	std::unique_ptr<ofsim_renderer::ModelRenderer> modelRenderer;	
	std::unique_ptr<Smoke> smoke;

	// orientation:
	glm::dvec3 position{ dvec3(0.0, 0.0, 0.0) };
	glm::dvec3 initialPosition{ dvec3(0.0, 0.0, 0.0) };
	glm::dvec3 towards{ dvec3(0.0, 0.0, 0.0) };
	glm::dvec3 rotation{ dvec3(0.0, 0.0, 0.0) };
			
	// physics:
	double size;
	glm::dvec3 velocity{ dvec3(0.0, 0.0, 0.0) };
	double mass{ 10.0 };
	double thrustMagnitude;	
};