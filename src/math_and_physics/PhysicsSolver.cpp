#include "PhysicsSolver.h"

#include <iostream>

using namespace ofsim_math_and_physics;

// the class and the method descriptions are in the header file

PhysicsSolver::PhysicsSolver(Rocket& _rocket, i32 _MS_PER_UPDATE)
	: rocket(_rocket)
{
	MS_PER_UPDATE = _MS_PER_UPDATE;
    thrustMagnitude = 0.01; //0.24;
}

void PhysicsSolver::changeInitialAltitudeOrientation(
    CelestialBodyType _celestialBodyType, 
    f64 _celestialBodySize,
    dvec3 _towards)
{
	altitudeOrientation = _celestialBodyType;
	celestialBodySize = _celestialBodySize;

    towards = _towards; 
    lastPos = rocket.getPosition();

    thrustCutOff = false;

    dvec3 direction = normalize(rocket.getPosition() - towards);
    quat qlook = Geometry::gLookAt(direction, dvec3(0.0, 1.0, 0.0));
    dvec3 rotation = eulerAngles(qlook) * 180.0f / 3.14159265358979323846f;

    thrustVector = direction * thrustMagnitude; 
    
    initialTowards = towards;

    rocket.updateRotation(rotation);
    rocket.updateTowards(towards);
}

u64 PhysicsSolver::calculateForces(u64 timeInterval)
{
    altitude = calculateAltitude();
    if (altitude > 0.2)
    {
        while (timeInterval > MS_PER_UPDATE)
        {
            double mass = rocket.getMass();

            if (!thrustCutOff)
            {
                addForce(thrustVector);
            }

            if (mass < 3.0)
            {
                thrustCutOff = true;
            }
            
            if (!thrustCutOff)
            {
                rocket.updateMass(mass -= 0.0004);
            }

            updatePhysics(MS_PER_UPDATE / 1000.0f);
            timeInterval -= MS_PER_UPDATE;

            deltaP = rocket.getPosition() - lastPos;
            lastPos = rocket.getPosition();
            towards += deltaP;

            calculateAtmosphereGradient();
            calculateAtmosphericDragForce();
        }
    }

    return timeInterval;
}

void PhysicsSolver::updatePhysics(f64 deltaTime)
{
    dvec3 velocity = rocket.getVelocity();
    dvec3 position = rocket.getPosition();

    dvec3 gravityForceVector = normalize(rocket.getPosition() - celestialBodyCenter(celestialBodySize)) * GConst;

    dvec3 sumOfForces = dvec3(0.0);
    for (unsigned int i = 0; i < forces.size(); i++)
    {
        sumOfForces += forces[i];
    }

    velocity += (gravityForceVector + (sumOfForces / rocket.getMass())) * deltaTime;
    position += velocity * deltaTime;

    rocket.updatePosition(position);
    rocket.updateVelocity(velocity);

    resetForces();
}

void PhysicsSolver::predictTrajectory(u64 elapsedTime)
{   
    trajectoryPredictionX.clear();
    trajectoryPredictionY.clear();
    trajectoryPredictionZ.clear();

    dvec3 currentVelocity = rocket.getVelocity();
    dvec3 position = rocket.getPosition();

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

void PhysicsSolver::addForce(vec3 force)
{
    forces.push_back(force);
}

void PhysicsSolver::resetForces()
{
    forces.clear();
}

void PhysicsSolver::reset()
{
    resetForces();
    thrustMagnitude = 0.01;
    thrustCutOff = true;

    rocket.reset();
    changeInitialAltitudeOrientation(CelestialBodyType::planet, 3185.0, initialTowards);
        
    altitude = calculateAltitude();
    calculateAtmosphereGradient();
    calculateAtmosphericDragForce();
}

void PhysicsSolver::updateRocketOrientation(i32 _lastKeyPressed)
{
    lastKeyPressed = _lastKeyPressed;
    dvec3 orgRotation = rocket.getRotation();
    if (lastKeyPressed != 0)
    {  
        f64 f { 0.2 };
        f64 x { 0 }, y { 0 }, z { 0 };
        if (lastKeyPressed == 265) //GLFW_KEY_UP
        {
            x += f;
        }

        if (lastKeyPressed == 264) //GLFW_KEY_DOWN
        {
            x -= f;
        }

        if (lastKeyPressed == 262) //GLFW_KEY_RIGHT
        {
            y -= f;
        }

        if (lastKeyPressed == 263) //GLFW_KEY_LEFT
        {
            y += f;
        }
       
        dvec3 deltaRotation = dvec3(x, y, z);
        orgRotation += deltaRotation;        
        rotateVectors(orgRotation, deltaRotation);
    }
}

void PhysicsSolver::updateThrustMagnitude(f64 newMagintude)
{
    thrustMagnitude = newMagintude;
    thrustVector = normalize(thrustVector);
    thrustVector *= thrustMagnitude;
    thrustCutOff = false;
    
    rocket.updateThrustMagnitude(newMagintude);
    
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

    rocket.updateRotation(newRotation);
}

void PhysicsSolver::rotateRocket(dvec3 deltaRotation)
{
    dvec3 orgRotation = rocket.getRotation();
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
    if (length(rocket.getVelocity()) > 0.0) 
    {
        dvec3 forceDirection = normalize(rocket.getVelocity()) * -1.0;
        f64 velocityMagnitude = 1.1 * length(rocket.getVelocity());
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
        return length(rocket.getPosition() - celestialBodyCenter(celestialBodySize)) - celestialBodySize + 0.5;
    }

    return 0.0;
}