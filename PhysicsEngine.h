#pragma once

#include "Geometry.h"
#include "CelestialBody.h"
#include "Rocket.h"

class PhysicsEngine
{
public:
	PhysicsEngine(Rocket& _rocket, int _MS_PER_UPDATE);

	void changeAltitudeOrientation(
		CelestialBodyType _celestialBodyType, 
		double _celestialBodySize,
		glm::dvec3 _towards);

	unsigned __int64 calculateForces(unsigned __int64 timeInterval);

	void updateKeyPressed(int _lastKeyPressed);

	double getAltitude();
	double getThrustMagnitude();

	float* atmosphereRgb();

	double getAtmosphereDragForceMagnitude();

	~PhysicsEngine();

private:
	void updatePhysics(double deltaTime);
	void addForce(glm::vec3 force);
	void resetForces();

	void calculateAtmosphereGradient();
	void calculateAtmosphericDragForce();
	
	double calculateAltitude();
	glm::dvec3 changeRocketRotation();
	
	glm::dvec3 celestialBodyCenter(double bodySize);

	std::vector<glm::dvec3> forces;

	CelestialBodyType altitudeOrientation;
	Rocket& rocket;
	
	//earth's atmosphere gradient color:
	float r = 0.25, g = 0.55, b = 0.75;
	float orr = 0.25, og = 0.55, ob = 0.75;

	double altitude;
	double celestialBodySize;
	double atmosphereDragForceMagnitude = 0.0;

	int MS_PER_UPDATE;

	glm::dvec3 thrustVector;
	double thrustMagnitude;

	bool thrustCutOff;

	int lastKeyPressed;

	glm::dvec3 towards;
	glm::dvec3 lastPos;

	double theta = 30.0; // todo: znaleŸæ wartoœci pocz¹tkowe
	double phi = 30.0;
};