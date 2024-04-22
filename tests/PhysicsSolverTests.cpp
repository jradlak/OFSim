#include <gtest/gtest.h>

#include "../src/math_and_physics/PhysicsSolver.h"

using namespace ofsim_math_and_physics;
using namespace ofsim_world;

TEST(PhysicsSolverTests, primitiveTest1)
{   
    dvec3 position{ dvec3(0.0, 0.0, 0.0) };
    dvec3 initialPosition{ dvec3(0.0, 0.0, 0.0) };
    dvec3 towards{ dvec3(0.0, 0.0, 0.0) };
    dvec3 rotation{ dvec3(0.0, 0.0, 0.0) };
    dvec3 velocity{ dvec3(0.0, 0.0, 0.0) };

    f64 size{ 1.0 };
    f64 mass{ 1.0 };
    f64 thrustMagnitude{ 1.0 };

    RocketPhysicalProperties rocketProperties 
    {
        position,
        initialPosition,
        towards,
        rotation,
        velocity,
        size,
        mass,
        thrustMagnitude
    };

    auto physicsSolver{ PhysicsSolver(rocketProperties, 
        CelestialBodyType::planet,
        1.0,  12) };

    physicsSolver.establishInitialOrientation(dvec3(0.0, 0.0, 0.0));

    u64 timeInterval{ 12 };

    u64 forces{ physicsSolver.calculateForces(timeInterval) };

    EXPECT_EQ(forces, 12);
}
