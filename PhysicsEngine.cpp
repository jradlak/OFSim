#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine(Rocket& _rocket, int _MS_PER_UPDATE)
	: rocket(_rocket)
{
	MS_PER_UPDATE = MS_PER_UPDATE;
}

void PhysicsEngine::changeAltitudeOrientation(CelestialBodyType _celestialBodyType, double _celestialBodySize)
{
	altitudeOrientation = _celestialBodyType;
	celestialBodySize = _celestialBodySize;
}

void PhysicsEngine::calculateForces(unsigned __int64 timeInterval)
{
    double altitude = calculateAltitude();
    if (altitude > 0.09)
    {
        glm::vec3 gravityForceVector = glm::normalize(rocket.getPosition() - celestialBodyCenter(celestialBodySize)) * -0.0981;
        rocket.addForce(gravityForceVector);

        if (altitude > 5.0)
        {
            thrustCutOff = true;
        }

        if (!thrustCutOff)
        {
            rocket.addForce(thrustVector);
        }

        while (timeInterval > MS_PER_UPDATE)
        {
            rocket.updatePhysics(MS_PER_UPDATE / 1000.0f);
            timeInterval -= MS_PER_UPDATE;
        }

        //reacalculate thrust vector direction:
        //towards += rocket.getPosition() / 1000.0;
        glm::dvec3 rotat = changeRocketRotation(rocket, thrustVector, towards);
        if (rotat != glm::dvec3(0.0))
        {
            rotation = rotat;
        }

        rocket.updateRotation(rotation);
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

glm::dvec3 celestialBodyCenter(double bodySize)
{
    return glm::dvec3(0.0, -bodySize, 0.0)
}

PhysicsEngine::~PhysicsEngine()
{
}
