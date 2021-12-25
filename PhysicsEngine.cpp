#include "PhysicsEngine.h"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp> 

PhysicsEngine::PhysicsEngine(Rocket& _rocket, int _MS_PER_UPDATE)
	: rocket(_rocket)
{
	MS_PER_UPDATE = _MS_PER_UPDATE;
}

void PhysicsEngine::changeAltitudeOrientation(
    CelestialBodyType _celestialBodyType, 
    double _celestialBodySize,
    glm::dvec3 _towards)
{
	altitudeOrientation = _celestialBodyType;
	celestialBodySize = _celestialBodySize;

    towards = _towards; //celestialBodyCenter(celestialBodySize);
    lastPos = rocket.getPosition();

    thrustCutOff = false;

    glm::dvec3 direction = glm::normalize(rocket.getPosition() - towards);
    glm::quat qlook = Geometry::gLookAt(direction, glm::dvec3(0.0, 1.0, 0.0));
    glm::dvec3 rotation = glm::eulerAngles(qlook) * 180.0f / 3.14159f;

    thrustVector = direction * 0.64;

    rocket.updateRotation(rotation);
}

unsigned __int64 PhysicsEngine::calculateForces(unsigned __int64 timeInterval)
{
    altitude = calculateAltitude();
    if (altitude > 0.09)
    {
        glm::vec3 gravityForceVector = glm::normalize(rocket.getPosition() - celestialBodyCenter(celestialBodySize)) * -0.0981;
        addForce(gravityForceVector);

        if (altitude > 30.0)
        {
            thrustCutOff = true;
        }

        if (!thrustCutOff)
        {
            addForce(thrustVector);
        }

        while (timeInterval > MS_PER_UPDATE)
        {
            updatePhysics(MS_PER_UPDATE / 1000.0f);
            timeInterval -= MS_PER_UPDATE;
        }

        //recalculate thrust vector direction:
        glm::dvec3 rotation = rocket.getRotation();
        glm::dvec3 rotat = changeRocketRotation();
        if (rotat != glm::dvec3(0.0))
        {
            rotation = rotat;
        }

        glm::dvec3 deltaP = rocket.getPosition() - lastPos;
        lastPos = rocket.getPosition();
        towards += deltaP;

        rocket.updateRotation(rotation);
    }

    calculateAtmosphereGradient();

    return timeInterval;
}

void PhysicsEngine::updatePhysics(double deltaTime)
{
    glm::dvec3 velocity = rocket.getVelocity();
    glm::dvec3 position = rocket.getPosition();

    glm::dvec3 sumOfForces = glm::dvec3(0.0);
    for (unsigned int i = 0; i < forces.size(); i++)
    {
        sumOfForces += forces[i];
    }

    velocity += sumOfForces / rocket.getMass() * deltaTime;
    position += velocity * deltaTime;

    rocket.updatePosition(position);
    rocket.updateVelocity(velocity);

    resetForces();
}

void PhysicsEngine::addForce(glm::vec3 force)
{
    forces.push_back(force);
}

void PhysicsEngine::resetForces()
{
    forces.clear();
}

void PhysicsEngine::updateKeyPressed(int _lastKeyPressed)
{
    lastKeyPressed = _lastKeyPressed;
}

double PhysicsEngine::getAltitude()
{
    return altitude;
}

float* PhysicsEngine::atmosphereRgb()
{
    float rgb[] = { r, g, b };
    return rgb;
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

glm::dvec3 PhysicsEngine::changeRocketRotation()
{    
    double factor = 0.8;
   
    if (lastKeyPressed == 265) //GLFW_KEY_UP
    {
        theta += factor;        
    }

    if (theta > 359.0)
    {
        theta = 359.0;
    }

    if (lastKeyPressed == 264) //GLFW_KEY_DOWN
    {
        theta -= factor;        
    }

    if (theta < 0.1)
    {
        theta = 0.1;
    }

    if (lastKeyPressed == 262) //GLFW_KEY_RIGHT
    {
        phi -= factor;        
    }

    if (phi < 0.1)
    {
        phi = 0.1;
    }

    if (lastKeyPressed == 263) //GLFW_KEY_LEFT
    {
        phi += factor;        
    }

    if (phi > 359.0)
    {
        phi = 359.0;
    }
   
    if (lastKeyPressed != 0)
    {
        glm::dvec3 pos = rocket.getPosition();
        double r = glm::length(pos - towards);
        double x = r * cos(glm::radians(phi)) * sin(glm::radians(theta));
        double z = r * sin(glm::radians(phi)) * sin(glm::radians(theta));
        double y = r * cos(glm::radians(theta));

        towards.x = pos.x - x;
        towards.y = pos.y - y;
        towards.z = pos.z - z;

        glm::dvec3 direction = glm::normalize(rocket.getPosition() - towards);
        glm::quat qlook = Geometry::gLookAt(direction, glm::dvec3(0.0, 1.0, 0.0));
        glm::dvec3 rotation = glm::eulerAngles(qlook) * 180.0f / 3.14159f;

        thrustVector = direction * 0.64;
        lastKeyPressed = 0;

        return rotation;
    }

    return glm::dvec3(0);
}

PhysicsEngine::~PhysicsEngine()
{
}