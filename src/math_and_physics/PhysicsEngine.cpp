#include "PhysicsEngine.h"

#include <iostream>

PhysicsEngine::PhysicsEngine(Rocket& _rocket, int _MS_PER_UPDATE)
	: rocket(_rocket)
{
	MS_PER_UPDATE = _MS_PER_UPDATE;
    thrustMagnitude = 0.01; //0.24;
}

void PhysicsEngine::changeAltitudeOrientation(
    CelestialBodyType _celestialBodyType, 
    double _celestialBodySize,
    glm::dvec3 _towards)
{
	altitudeOrientation = _celestialBodyType;
	celestialBodySize = _celestialBodySize;

    towards = _towards; 
    lastPos = rocket.getPosition();

    thrustCutOff = false;

    glm::dvec3 direction = glm::normalize(rocket.getPosition() - towards);
    glm::quat qlook = Geometry::gLookAt(direction, glm::dvec3(0.0, 1.0, 0.0));
    glm::dvec3 rotation = glm::eulerAngles(qlook) * 180.0f / 3.14159265358979323846f;

    thrustVector = direction * thrustMagnitude; 
    
    initialTowards = towards;

    rocket.updateRotation(rotation);
    rocket.updateTowards(towards);
}

unsigned long long PhysicsEngine::calculateForces(unsigned long long timeInterval)
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

void PhysicsEngine::updatePhysics(double deltaTime)
{
    glm::dvec3 velocity = rocket.getVelocity();
    glm::dvec3 position = rocket.getPosition();

    glm::dvec3 gravityForceVector = glm::normalize(rocket.getPosition() - celestialBodyCenter(celestialBodySize)) * GConst;

    glm::dvec3 sumOfForces = glm::dvec3(0.0);
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

void PhysicsEngine::predictTrajectory(unsigned long long elapsedTime)
{   
    trajectoryPredictionX.clear();
    trajectoryPredictionY.clear();
    trajectoryPredictionZ.clear();

    glm::dvec3 currentVelocity = rocket.getVelocity();
    glm::dvec3 position = rocket.getPosition();

    elapsedTime /= 1000;
    int n = 512;
    double currentTime = 4000.0 - elapsedTime;
    double deltaTime = (double)currentTime / n;

    for (int index = 0; index < n; index++)
    {
        glm::dvec3 gravityForceVector = glm::normalize(position - celestialBodyCenter(celestialBodySize)) * GConst;

        currentVelocity += gravityForceVector * deltaTime;
        position += currentVelocity * deltaTime;
        currentTime += deltaTime;

        double altitude = glm::length(position - celestialBodyCenter(celestialBodySize)) - celestialBodySize + 0.5;
        if (altitude < 0.3)
        {
            break;
        }

        trajectoryPredictionX.push_back(position.x);
        trajectoryPredictionY.push_back(position.y);
        trajectoryPredictionZ.push_back(position.z);

        velocityMagnitude.push_back(glm::length(currentVelocity));
    }
}

void PhysicsEngine::addForce(glm::vec3 force)
{
    forces.push_back(force);
}

void PhysicsEngine::resetForces()
{
    forces.clear();
}

void PhysicsEngine::reset()
{
    resetForces();
    thrustMagnitude = 0.01;
    thrustCutOff = true;

    rocket.reset();
    changeAltitudeOrientation(CelestialBodyType::planet, 3185.0, initialTowards);
        
    altitude = calculateAltitude();
    calculateAtmosphereGradient();
    calculateAtmosphericDragForce();
}

void PhysicsEngine::updateKeyPressed(int _lastKeyPressed)
{
    lastKeyPressed = _lastKeyPressed;
    glm::dvec3 orgRotation = rocket.getRotation();
    if (lastKeyPressed != 0)
    {  
        double f = 0.2;
        double x = 0, y = 0, z = 0;
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
       
        glm::dvec3 deltaRotation = glm::dvec3(x, y, z);
        orgRotation += deltaRotation;        
        rotateVectors(orgRotation, deltaRotation);
    }
}

void PhysicsEngine::updateThrustMagnitude(double newMagintude)
{
    thrustMagnitude = newMagintude;
    thrustVector = glm::normalize(thrustVector);
    thrustVector *= thrustMagnitude;
    thrustCutOff = false;
    
    rocket.updateThrustMagnitude(newMagintude);
    
    if (newMagintude <= 0.01)
    {
        thrustCutOff = true;
    }        
}

void PhysicsEngine::rotateVectors(glm::dvec3 newRotation, glm::dvec3 deltaRotation)
{    
    // rotate thrust vector:
    if (deltaRotation.x != 0)
    {
        thrustVector = Geometry::rotateVector(thrustVector, glm::dvec3(1.0, 0.0, 0.0), deltaRotation.x);
    }

    if (deltaRotation.y != 0)
    {
        thrustVector = Geometry::rotateVector(thrustVector, glm::dvec3(0.0, 1.0, 0.0), deltaRotation.y);
    }

    if (deltaRotation.z != 0)
    {
        thrustVector = Geometry::rotateVector(thrustVector, glm::dvec3(0.0, 0.0, 1.0), deltaRotation.z);
    }

    rocket.updateRotation(newRotation);
}

void PhysicsEngine::rotateRocket(glm::dvec3 deltaRotation)
{
    glm::dvec3 orgRotation = rocket.getRotation();
    orgRotation += deltaRotation;
    rotateVectors(orgRotation, deltaRotation);
}

double PhysicsEngine::getAltitude()
{
    return altitude;
}

double PhysicsEngine::getThrustMagnitude()
{
    return thrustMagnitude;
}

std::vector<float> PhysicsEngine::atmosphereRgb()
{
    std::vector<float> rgb = { r, g, b };
    return rgb;
}

double PhysicsEngine::getAtmosphereDragForceMagnitude()
{
    if (altitude > 98.0)
    {
        return 0.0;
    }
    else
    {
        return atmosphereDragForceMagnitude;
    }
}

void PhysicsEngine::calculateAtmosphereGradient()
{
    //atmosphere gradient simulation:
    float factor = 0; 
    if (altitude > 4.0) {
        factor = 0.001 * altitude * altitude;
    }
    b = ob - factor;
    if (b < 0)
    {
        b = 0;
    }

    r = orr - factor;
    if (r < 0)
    {
        r = 0;
    }

    g = og - factor;
    if (g < 0)
    {
        g = 0;
    }
}

void PhysicsEngine::calculateAtmosphericDragForce()
{
    if (glm::length(rocket.getVelocity()) > 0.0) 
    {
        glm::dvec3 forceDirection = glm::normalize(rocket.getVelocity()) * -1.0;
        double velocityMagnitude = 1.1 * glm::length(rocket.getVelocity());
        double altitudeMagnitude = 1.0 / (altitude * 1.2);
        if (altitude > 20.0)
        {
            altitudeMagnitude *= 1.0 / altitude;
        }

        glm::dvec3 dragForce = forceDirection * velocityMagnitude * altitudeMagnitude;

        if (glm::length(dragForce) > 0.0001 && altitude < 98.0)
        {            
            addForce(dragForce);
        }

        atmosphereDragForceMagnitude = glm::length(dragForce);
    }
}

double PhysicsEngine::calculateAltitude()
{
    if (altitudeOrientation == planet)
    {
        return glm::length(rocket.getPosition() - celestialBodyCenter(celestialBodySize)) - celestialBodySize + 0.5;
    }

    return 0.0;
}

glm::dvec3 PhysicsEngine::celestialBodyCenter(double bodySize)
{
    return glm::dvec3(0.0, -bodySize, 0.0);
}

PhysicsEngine::~PhysicsEngine()
{
}