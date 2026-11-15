#include <gtest/gtest.h>

#include "../src/physics/PhysicsSolver.h"

namespace
{
using namespace ofsim_math_and_physics;
using namespace ofsim_world;

RocketPhysicalProperties initialProperties()
{
    RocketPhysicalProperties properties{};
    properties.position = SolarSystemConstants::earthPos +
                          dvec3(0.0, 2.0 * SolarSystemConstants::earthSize + 100.0, 0.0);
    properties.velocity = dvec3(0.0);
    properties.mass = rocket_initial_mass;
    // The solver must overwrite stale telemetry at construction and reset.
    properties.thrustMagnitude = 99.0;
    return properties;
}

TEST(PhysicsSolverTests, FirstStepWithoutThrustCommandIsCoasting)
{
    auto properties = initialProperties();
    const auto initialPosition = properties.position;
    PhysicsSolver physics(properties, planet, SolarSystemConstants::earthSize, 12);
    physics.establishInitialOrientation(properties.position, 0.0, 0.0);
    EXPECT_DOUBLE_EQ(properties.thrustMagnitude, physics.thrustMagnitude);
    EXPECT_NEAR(length(physics.thrustVector), physics.thrustMagnitude, 1e-12);
    EXPECT_EQ(physics.deltaP, dvec3(0.0));

    // More than one interval is required to actually enter the integration loop.
    EXPECT_EQ(physics.calculateForces(13), 1);
    EXPECT_DOUBLE_EQ(properties.mass, rocket_initial_mass);
    EXPECT_DOUBLE_EQ(properties.velocity.x, 0.0);
    EXPECT_DOUBLE_EQ(properties.velocity.z, 0.0);
    EXPECT_NEAR(properties.velocity.y, SolarSystemConstants::GConst * (12 / 1000.0f), 1e-12);
    EXPECT_LT(properties.position.y, initialPosition.y);
    EXPECT_EQ(physics.deltaP, properties.position - initialPosition);
}

TEST(PhysicsSolverTests, ResetAfterPoweredFlightRestoresInitialEngineStateAndDisplacement)
{
    auto properties = initialProperties();
    PhysicsSolver physics(properties, planet, SolarSystemConstants::earthSize, 12);
    physics.establishInitialOrientation(properties.position, 0.0, 0.0);
    const auto initialThrust = physics.thrustMagnitude;
    const auto initialAltitude = physics.altitude;
    physics.updateThrustMagnitude(0.24);
    physics.calculateForces(13);
    EXPECT_LT(properties.mass, rocket_initial_mass);

    properties = initialProperties();
    physics.reset();
    EXPECT_DOUBLE_EQ(physics.thrustMagnitude, initialThrust);
    EXPECT_DOUBLE_EQ(properties.thrustMagnitude, initialThrust);
    EXPECT_NEAR(length(physics.thrustVector), initialThrust, 1e-12);
    EXPECT_DOUBLE_EQ(physics.altitude, initialAltitude);
    EXPECT_EQ(physics.deltaP, dvec3(0.0));
    const auto initialPosition = properties.position;
    physics.calculateForces(13);
    EXPECT_DOUBLE_EQ(properties.mass, rocket_initial_mass);
    EXPECT_NEAR(properties.velocity.y, SolarSystemConstants::GConst * (12 / 1000.0f), 1e-12);
    EXPECT_EQ(physics.deltaP, properties.position - initialPosition);
}

TEST(PhysicsSolverTests, ResetAfterZeroThrustAllowsEngineToStartAgain)
{
    auto properties = initialProperties();
    PhysicsSolver physics(properties, planet, SolarSystemConstants::earthSize, 12);
    physics.establishInitialOrientation(properties.position, 0.0, 0.0);
    physics.updateThrustMagnitude(0.0);
    physics.reset();
    physics.updateThrustMagnitude(0.24);
    EXPECT_NEAR(length(physics.thrustVector), 0.24, 1e-12);
    physics.calculateForces(13);
    EXPECT_LT(properties.mass, rocket_initial_mass);
    EXPECT_GT(properties.velocity.y, 0.0);
}
} // namespace
