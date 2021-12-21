#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "CelestialBody.h"
#include "Rocket.h"

class PhysicsEngine
{
public:
	PhysicsEngine(Rocket& _rocket, int _MS_PER_UPDATE);

	void changeAltitudeOrientation(CelestialBodyType _celestialBodyType, double _celestialBodySize);

	void calculateForces(unsigned __int64 timeInterval);

	~PhysicsEngine();

private:
	void calculateAtmosphereGradient();
	double calculateAltitude();

	CelestialBodyType altitudeOrientation;
	Rocket& rocket;
	
	//earth's atmosphere gradient color:
	float r = 0.25, g = 0.55, b = 0.75;
	float orr = 0.25, og = 0.55, ob = 0.75;

	double celestialBodySize;
	int MS_PER_UPDATE;

	glm::dvec3 thrustVector;
	bool thrustCutOff;
};