#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp> 

#include "Geometry.h"
#include "../world/solar_system/CelestialBody.h"
#include "../world/Rocket.h"

class PhysicsEngine
{
public:
	PhysicsEngine(Rocket& _rocket, int _MS_PER_UPDATE);

	void changeAltitudeOrientation(
		CelestialBodyType _celestialBodyType, 
		double _celestialBodySize,
		glm::dvec3 _towards);

	unsigned long long calculateForces(unsigned long long timeInterval);

	void updateKeyPressed(int _lastKeyPressed);
	void updateThrustMagnitude(double newMagintude);
	void rotateVectors(glm::dvec3 newRotation, glm::dvec3 deltaRotation);
	void rotateRocket(glm::dvec3 deltaRotation);

	void predictTrajectory(unsigned long long elapsedTime);

	void resetForces();
	void reset();

	double getAltitude();
	double getThrustMagnitude();

	std::vector<float> atmosphereRgb();

	double getAtmosphereDragForceMagnitude();

	glm::dvec3 getDeltaPosition() { return deltaP; }

	std::vector<double> getTrajectoryPredictionX() { return trajectoryPredictionX; }
	std::vector<double> getTrajectoryPredictionY() { return trajectoryPredictionY; }
	std::vector<double> getTrajectoryPredictionZ() { return trajectoryPredictionZ; }

	~PhysicsEngine();

private:
	void updatePhysics(double deltaTime);
	void addForce(glm::vec3 force);

	void calculateAtmosphereGradient();
	void calculateAtmosphericDragForce();
	
	double calculateAltitude();
	
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

	double GConst = -0.00981;

	int MS_PER_UPDATE;

	glm::dvec3 thrustVector;
	double thrustMagnitude;

	bool thrustCutOff;

	int lastKeyPressed;
	bool mustRecalculateVectors = false;

	glm::dvec3 towards;
	glm::dvec3 lastPos;

	glm::dvec3 initialTowards;

	glm::dvec3 deltaP;

	double theta = 30.0; // todo: znale�� warto�ci pocz�tkowe
	double phi = 30.0;

	std::vector<double> trajectoryPredictionX;
	std::vector<double> trajectoryPredictionY;
	std::vector<double> trajectoryPredictionZ;
	std::vector<double> velocityMagnitude;
};