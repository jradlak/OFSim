#include <gtest/gtest.h>

#include <cmath>
#include <limits>

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

TEST(PhysicsSolverTests, ZeroThrustCanRestartRepeatedlyWithoutReset)
{
    auto properties = initialProperties();
    PhysicsSolver physics(properties, planet, SolarSystemConstants::earthSize, 12);
    physics.establishInitialOrientation(properties.position, 0.0, 0.0);

    for (int iteration = 0; iteration < 3; ++iteration)
    {
        physics.updateThrustMagnitude(0.0);
        EXPECT_EQ(physics.thrustVector, dvec3(0.0));
        const auto massBeforeCoasting = properties.mass;
        physics.calculateForces(13);
        EXPECT_DOUBLE_EQ(properties.mass, massBeforeCoasting);

        physics.updateThrustMagnitude(0.24);
        EXPECT_NEAR(length(physics.thrustVector - dvec3(0.0, 0.24, 0.0)), 0.0, 1e-12);
        EXPECT_DOUBLE_EQ(properties.thrustMagnitude, 0.24);
        physics.calculateForces(13);
        EXPECT_LT(properties.mass, massBeforeCoasting);
        for (int axis = 0; axis < 3; ++axis)
        {
            EXPECT_TRUE(std::isfinite(properties.position[axis]));
            EXPECT_TRUE(std::isfinite(properties.velocity[axis]));
        }
    }
}

TEST(PhysicsSolverTests, RotationAtZeroThrustIsUsedWhenEngineRestarts)
{
    const dvec3 rotations[] = {{90.0, 0.0, 0.0}, {0.0, 0.0, 90.0}, {90.0, 90.0, 0.0}};
    const dvec3 directions[] = {{0.0, 0.0, 1.0}, {-1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}};
    for (int index = 0; index < 3; ++index)
    {
        SCOPED_TRACE(index);
        auto properties = initialProperties();
        PhysicsSolver physics(properties, planet, SolarSystemConstants::earthSize, 12);
        physics.establishInitialOrientation(properties.position, 0.0, 0.0);
        physics.updateThrustMagnitude(0.0);
        physics.rotateRocket(rotations[index]);
        EXPECT_EQ(physics.thrustVector, dvec3(0.0));
        EXPECT_NEAR(length(physics.getThrustDirection() - directions[index]), 0.0, 1e-12);

        physics.updateThrustMagnitude(0.24);
        EXPECT_NEAR(length(physics.thrustVector - directions[index] * 0.24), 0.0, 1e-12);
        physics.calculateForces(13);
        EXPECT_GT(glm::dot(properties.velocity, directions[index]), 0.0);
    }
}

TEST(PhysicsSolverTests, OrientationAndResetPreserveDirectionWithEngineOff)
{
    auto properties = initialProperties();
    properties.position = dvec3(length(properties.position), 0.0, 0.0);
    PhysicsSolver physics(properties, planet, SolarSystemConstants::earthSize, 12);
    physics.updateThrustMagnitude(0.0);
    physics.establishInitialOrientation(properties.position, 0.0, 90.0);
    EXPECT_EQ(physics.getThrustDirection(), dvec3(1.0, 0.0, 0.0));
    physics.rotateRocket(dvec3(0.0, 90.0, 0.0));
    physics.reset();
    EXPECT_NEAR(length(physics.getThrustDirection() - dvec3(0.0, 0.0, -1.0)), 0.0, 1e-12);
    EXPECT_NEAR(length(physics.thrustVector), physics.thrustMagnitude, 1e-12);
    physics.updateThrustMagnitude(0.24);
    EXPECT_NEAR(length(physics.thrustVector - dvec3(0.0, 0.0, -0.24)), 0.0, 1e-12);
}

TEST(PhysicsSolverTests, PredictionAtOrBeyondLimitClearsPreviousResults)
{
    auto properties = initialProperties();
    PhysicsSolver physics(properties, planet, SolarSystemConstants::earthSize, 12);
    const u64 elapsedTimes[] = {8000000, 8000001, 8001000, std::numeric_limits<u64>::max()};
    for (const auto elapsed : elapsedTimes)
    {
        SCOPED_TRACE(elapsed);
        physics.predictTrajectory(7999000);
        ASSERT_FALSE(physics.trajectoryPredictionX.empty());
        physics.predictTrajectory(elapsed);
        EXPECT_TRUE(physics.trajectoryPredictionX.empty());
        EXPECT_TRUE(physics.trajectoryPredictionY.empty());
        EXPECT_TRUE(physics.trajectoryPredictionZ.empty());
    }
}

TEST(PhysicsSolverTests, PredictionUsesTheLastMillisecondWithoutChangingRocketState)
{
    auto properties = initialProperties();
    properties.velocity = dvec3(1.0, 0.0, 0.0);
    const auto initialPosition = properties.position;
    PhysicsSolver physics(properties, planet, SolarSystemConstants::earthSize, 12);
    physics.predictTrajectory(7999999);
    ASSERT_EQ(physics.trajectoryPredictionX.size(), 2048);
    ASSERT_EQ(physics.trajectoryPredictionY.size(), physics.trajectoryPredictionX.size());
    ASSERT_EQ(physics.trajectoryPredictionZ.size(), physics.trajectoryPredictionX.size());
    EXPECT_NEAR(physics.trajectoryPredictionX.back(), 0.001, 1e-12);
    EXPECT_EQ(properties.position, initialPosition);
    EXPECT_EQ(properties.velocity, dvec3(1.0, 0.0, 0.0));
    EXPECT_DOUBLE_EQ(properties.mass, rocket_initial_mass);
    for (std::size_t index = 0; index < physics.trajectoryPredictionX.size(); ++index)
    {
        EXPECT_TRUE(std::isfinite(physics.trajectoryPredictionX[index]));
        EXPECT_TRUE(std::isfinite(physics.trajectoryPredictionY[index]));
        EXPECT_TRUE(std::isfinite(physics.trajectoryPredictionZ[index]));
    }
}

TEST(PhysicsSolverTests, RepeatedPredictionsReplaceResultsAndResetClearsThem)
{
    auto properties = initialProperties();
    properties.position.y = 1000000.0;
    properties.velocity.x = 1.0;
    PhysicsSolver physics(properties, planet, SolarSystemConstants::earthSize, 12);
    physics.predictTrajectory(0);
    ASSERT_EQ(physics.trajectoryPredictionX.size(), 2048);
    const auto x = physics.trajectoryPredictionX;
    const auto y = physics.trajectoryPredictionY;
    const auto z = physics.trajectoryPredictionZ;
    physics.predictTrajectory(0);
    EXPECT_EQ(physics.trajectoryPredictionX, x);
    EXPECT_EQ(physics.trajectoryPredictionY, y);
    EXPECT_EQ(physics.trajectoryPredictionZ, z);

    properties.velocity.x = -1.0;
    physics.predictTrajectory(7999000);
    ASSERT_FALSE(physics.trajectoryPredictionX.empty());
    EXPECT_NEAR(physics.trajectoryPredictionX.back(), -1.0, 1e-8);
    physics.reset();
    EXPECT_TRUE(physics.trajectoryPredictionX.empty());
    EXPECT_TRUE(physics.trajectoryPredictionY.empty());
    EXPECT_TRUE(physics.trajectoryPredictionZ.empty());
}
} // namespace
