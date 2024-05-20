#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "PhysicsSolver.h"

#include <iostream>

using namespace ofsim_math_and_physics;
using namespace ofsim_world;

// the class and the method descriptions are in the header file

PhysicsSolver::PhysicsSolver(
    RocketPhysicalProperties& _rocketProperties,
    CelestialBodyType _celestialBodyType,
    f64 _celestialBodySize, 
    i32 _MS_PER_UPDATE)
	: rocketProperties(_rocketProperties), celestialBodyType(_celestialBodyType), celestialBodySize(_celestialBodySize),
    ms_per_update(_MS_PER_UPDATE), thrustMagnitude(0.01) {}

void PhysicsSolver::establishInitialOrientation(dvec3 rocketInitialPosition, const f64 _theta, const f64 _phi)
{	    
    dvec3 direction = normalize(rocketInitialPosition - celestialBodyCenter(celestialBodySize));

    thrustVector = direction * thrustMagnitude;
    rocketProperties.rotation = dvec3(_theta, 0, _phi); // -dvec3(0.0, 0, 12.0);
}

u64 PhysicsSolver::calculateForces(u64 timeInterval)
{
    altitude = calculateAltitude();
    if (altitude >= min_altitude)
    {
        while (timeInterval > ms_per_update)
        {
            double mass = rocketProperties.mass;

            if (!thrustCutOff)
            {
                addForce(thrustVector);
            }

            if (mass < min_mass)
            {
                updateThrustMagnitude(min_thrust);
            }
            
            if (!thrustCutOff)
            {
                rocketProperties.mass = (mass -= 0.0004);
            }

            updatePhysics(ms_per_update / 1000.0f);
            timeInterval -= ms_per_update;

            deltaP = rocketProperties.position - lastPos;
            lastPos = rocketProperties.position;            

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
   
    dvec3 gravityForceVector = normalize(rocketProperties.position - celestialBodyCenter(celestialBodySize))
        * SolarSystemConstants::GConst;

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
    i32 n = prediction_steps;
    f64 currentTime = 8000.0 - elapsedTime;
    f64 deltaTime = (f64)currentTime / n;

    for (u32 index = 0; index < n; index++)
    {
        dvec3 gravityForceVector = normalize(position - celestialBodyCenter(celestialBodySize))
            * SolarSystemConstants::GConst;

        currentVelocity += gravityForceVector * deltaTime;
        position += currentVelocity * deltaTime;
        currentTime += deltaTime;

        f64 altitude = length(position - celestialBodyCenter(celestialBodySize)) - celestialBodySize + 0.5;
        if (altitude < min_altitude)
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
    thrustMagnitude = min_thrust;
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

void PhysicsSolver::rotateRocketAndThrust(dvec3 deltaRotation)
{        
    // rotate thrust vector:
    if (deltaRotation.x != 0)
    {
        thrustVector = glm::rotateX(thrustVector, glm::radians(deltaRotation.x));
    }

    if (deltaRotation.y != 0)
    {
        thrustVector = glm::rotateY(thrustVector, glm::radians(deltaRotation.y));
    }

    if (deltaRotation.z != 0)
    {
        thrustVector = glm::rotateZ(thrustVector, glm::radians(deltaRotation.z));
    }
    
    // rotate rocket orientation:
    rocketProperties.rotation += deltaRotation;    
}

void PhysicsSolver::calculateAtmosphereGradient()
{
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
        if (altitude > dense_atmosphere_boundary)
        {
            altitudeMagnitude *= 1.0 / altitude;
        }

        dvec3 dragForce = forceDirection * velocityMagnitude * altitudeMagnitude;

        if (length(dragForce) > 0.0001 && altitude < upper_atmosphere_boundary)
        {            
            addForce(dragForce);
        }

        atmosphereDragForceMagnitude = length(dragForce);
    }
}

double PhysicsSolver::calculateAltitude()
{
    if (celestialBodyType == planet)
    {
        return length(rocketProperties.position - celestialBodyCenter(celestialBodySize)) - celestialBodySize * 2.0 + 0.5;
    }

    return 0.0;
}
