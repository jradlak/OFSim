#include "PhysicsSolver.h"

#include <iostream>

using namespace ofsim_math_and_physics;

// the class and the method descriptions are in the header file

PhysicsSolver::PhysicsSolver(RocketPhysicalProperties& _rocketProperties, i32 _MS_PER_UPDATE)
	: rocketProperties(_rocketProperties), MS_PER_UPDATE(_MS_PER_UPDATE), thrustMagnitude(0.01) {}

void PhysicsSolver::changeInitialAltitudeOrientation(
    CelestialBodyType _celestialBodyType, 
    f64 _celestialBodySize,
    dvec3 _towards)
{
	altitudeOrientation = _celestialBodyType;
	celestialBodySize = _celestialBodySize;

    towards = _towards; 
    lastPos = rocketProperties.position;

    thrustCutOff = false;

    dvec3 direction = normalize(rocketProperties.position - towards);
    quat qlook = Geometry::gLookAt(direction, dvec3(0.0, 1.0, 0.0));
    dvec3 rotation = eulerAngles(qlook) * 180.0f / 3.14159265358979323846f;

    thrustVector = direction * thrustMagnitude; 
    
    initialTowards = towards;

    rocketProperties.rotation = rotation;
    rocketProperties.towards = towards;    
}

u64 PhysicsSolver::calculateForces(u64 timeInterval)
{
    altitude = calculateAltitude();
    if (altitude > 0.2)
    {
        while (timeInterval > MS_PER_UPDATE)
        {
            double mass = rocketProperties.mass;

            if (!thrustCutOff)
            {
                addForce(thrustVector);
            }

            if (mass < 3.0)
            {
                updateThrustMagnitude(0.0001);                
            }
            
            if (!thrustCutOff)
            {
                rocketProperties.mass = (mass -= 0.0004);
            }

            updatePhysics(MS_PER_UPDATE / 1000.0f);
            timeInterval -= MS_PER_UPDATE;

            deltaP = rocketProperties.position - lastPos;
            lastPos = rocketProperties.position;
            towards += deltaP;

            calculateAtmosphereGradient();
            calculateAtmosphericDragForce();
        }
    }

    return timeInterval;
}

void PhysicsSolver::updatePhysics(f64 deltaTime)
{
    dvec3 &velocity = rocketProperties.velocity;
    dvec3 &position = rocketProperties.position;

    dvec3 gravityForceVector = normalize(rocketProperties.position - celestialBodyCenter(celestialBodySize)) * GConst;

    dvec3 sumOfForces = dvec3(0.0);
    
    for (unsigned int i = 0; i < forces.size(); i++)
    {
        sumOfForces += forces[i];
    }

    velocity += (gravityForceVector + (sumOfForces / rocketProperties.mass)) * deltaTime;
    position += velocity * deltaTime;

    resetForces();
}

void PhysicsSolver::predictTrajectory(u64 elapsedTime)
{   
    trajectoryPredictionX.clear();
    trajectoryPredictionY.clear();
    trajectoryPredictionZ.clear();

    dvec3 currentVelocity = rocketProperties.velocity;
    dvec3 position = rocketProperties.position;

    elapsedTime /= 1000;
    i32 n = 512;
    f64 currentTime = 4000.0 - elapsedTime;
    f64 deltaTime = (f64)currentTime / n;

    for (u32 index = 0; index < n; index++)
    {
        dvec3 gravityForceVector = normalize(position - celestialBodyCenter(celestialBodySize)) * GConst;

        currentVelocity += gravityForceVector * deltaTime;
        position += currentVelocity * deltaTime;
        currentTime += deltaTime;

        f64 altitude = length(position - celestialBodyCenter(celestialBodySize)) - celestialBodySize + 0.5;
        if (altitude < 0.3)
        {
            break;
        }

        trajectoryPredictionX.push_back(position.x);
        trajectoryPredictionY.push_back(position.y);
        trajectoryPredictionZ.push_back(position.z);

        velocityMagnitude.push_back(length(currentVelocity));
    }
}

void PhysicsSolver::reset()
{
    resetForces();
    thrustMagnitude = 0.01;
    thrustCutOff = true;
                        
    altitude = 0.0;
    calculateAtmosphereGradient();
    calculateAtmosphericDragForce();
}

void PhysicsSolver::updateThrustMagnitude(f64 newMagintude)
{
    thrustMagnitude = newMagintude;
    thrustVector = normalize(thrustVector);
    thrustVector *= thrustMagnitude;
    thrustCutOff = false;
    
    rocketProperties.thrustMagnitude = newMagintude;
    
    if (newMagintude <= 0.01)
    {
        thrustCutOff = true;
    }        
}

void PhysicsSolver::rotateVectors(dvec3 newRotation, dvec3 deltaRotation)
{    
    // rotate thrust vector:
    if (deltaRotation.x != 0)
    {
        thrustVector = Geometry::rotateVector(thrustVector, dvec3(1.0, 0.0, 0.0), deltaRotation.x);
    }

    if (deltaRotation.y != 0)
    {
        thrustVector = Geometry::rotateVector(thrustVector, dvec3(0.0, 1.0, 0.0), deltaRotation.y);
    }

    if (deltaRotation.z != 0)
    {
        thrustVector = Geometry::rotateVector(thrustVector, dvec3(0.0, 0.0, 1.0), deltaRotation.z);
    }

    rocketProperties.rotation = newRotation;    
}

void PhysicsSolver::rotateRocket(dvec3 deltaRotation)
{
    dvec3 orgRotation = rocketProperties.rotation;
    orgRotation += deltaRotation;
    rotateVectors(orgRotation, deltaRotation);
}

void PhysicsSolver::calculateAtmosphereGradient()
{
    //atmosphere gradient simulation:
    f32 factor = altitude > 4.0 ? 0.001 * altitude * altitude : 0.0;
        
    b = ob - factor;
    b = b < 0 ? 0 : b;
    
    r = orr - factor;
    r = r < 0 ? 0 : r;
    
    g = og - factor;
    g = g < 0 ? 0 : g;    
}

void PhysicsSolver::calculateAtmosphericDragForce()
{
    if (length(rocketProperties.velocity) > 0.0) 
    {
        dvec3 forceDirection = normalize(rocketProperties.velocity) * -1.0;
        f64 velocityMagnitude = 1.1 * length(rocketProperties.velocity);
        f64 altitudeMagnitude = 1.0 / (altitude * 1.2);
        if (altitude > 20.0)
        {
            altitudeMagnitude *= 1.0 / altitude;
        }

        dvec3 dragForce = forceDirection * velocityMagnitude * altitudeMagnitude;

        if (length(dragForce) > 0.0001 && altitude < 98.0)
        {            
            addForce(dragForce);
        }

        atmosphereDragForceMagnitude = length(dragForce);
    }
}

double PhysicsSolver::calculateAltitude()
{
    if (altitudeOrientation == planet)
    {
        return length(rocketProperties.position - celestialBodyCenter(celestialBodySize)) - celestialBodySize + 0.5;
    }

    return 0.0;
}