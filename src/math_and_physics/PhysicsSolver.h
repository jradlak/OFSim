#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp> 

#include "Geometry.h"
#include "../world/solar_system/CelestialBody.h"
#include "../world/Rocket.h"

class PhysicsSolver
{
public:
	PhysicsSolver(Rocket& _rocket, i32 _MS_PER_UPDATE);

	// change state methods:
	void changeAltitudeOrientation(
		CelestialBodyType _celestialBodyType, 
		f64 _celestialBodySize,
		dvec3 _towards);

	u64 calculateForces(u64 timeInterval);

	void updateKeyPressed(i32 _lastKeyPressed);
	void updateThrustMagnitude(f64 newMagintude);
	void rotateVectors(dvec3 newRotation, dvec3 deltaRotation);
	void rotateRocket(dvec3 deltaRotation);

	void predictTrajectory(unsigned long long elapsedTime);

	void resetForces();
	void reset();

	//  simple properties getters:
	f64 getAltitude() { return altitude; }
	f64 getThrustMagnitude() { return thrustMagnitude; }	
	std::vector<f32> atmosphereRgb() { return { r, g, b }; }
	f64 getAtmosphereDragForceMagnitude() { return altitude > 98.0 ? 0.0 : atmosphereDragForceMagnitude; }
	dvec3 getDeltaPosition() { return deltaP; }
	std::vector<f64> getTrajectoryPredictionX() { return trajectoryPredictionX; }
	std::vector<f64> getTrajectoryPredictionY() { return trajectoryPredictionY; }
	std::vector<f64> getTrajectoryPredictionZ() { return trajectoryPredictionZ; }

	~PhysicsSolver() {}

private:

	const f32 orr { 0.25 }, og { 0.55 }, ob { 0.75 };

	//earth's atmosphere gradient color:
	f32 r { 0.25 }, g { 0.55 }, b { 0.75 };
	
	// PRIVATE VARIABLES:

	std::vector<dvec3> forces;

	CelestialBodyType altitudeOrientation;
	Rocket& rocket;
	
	f64 altitude;
	f64 celestialBodySize;
	f64 atmosphereDragForceMagnitude { 0.0 };

	f64 GConst = -0.00981;

	int MS_PER_UPDATE;

	dvec3 thrustVector;
	f64 thrustMagnitude;

	bool thrustCutOff;

	i32 lastKeyPressed;
	bool mustRecalculateVectors = false;

	dvec3 towards;
	dvec3 lastPos;

	dvec3 initialTowards;

	dvec3 deltaP;

	f64 theta = 30.0; // TODO: find better initial vaules
	f64 phi = 30.0;

	std::vector<f64> trajectoryPredictionX;
	std::vector<f64> trajectoryPredictionY;
	std::vector<f64> trajectoryPredictionZ;
	std::vector<f64> velocityMagnitude;

	// PRIVATE METHODS:

	void updatePhysics(f64 deltaTime);
	void addForce(vec3 force);

	void calculateAtmosphereGradient();
	void calculateAtmosphericDragForce();
	
	f64 calculateAltitude();
	
	dvec3 celestialBodyCenter(f64 bodySize) { return dvec3(0.0, -bodySize, 0.0); }
	
};	