#include <gtest/gtest.h>

#include <future>
#include <chrono>

#include "../src/infrastructure/EventProcessor.h"
#include "../src/physics/PhysicsSolver.h"
#include "../src/python_integration/PythonMachine.h"
#include "../src/simulation/ProgramRunner.h"
#include "../src/vmachine/VMachine.h"

namespace
{
using namespace ofsim_events;
using namespace ofsim_math_and_physics;
using namespace ofsim_world;
using namespace std::chrono_literals;

TelemetrySnapshot snapshot(u64 time)
{
    const double value = static_cast<double>(time);
    return {dvec3(value, value + 1, value + 2),
            dvec3(value + 3, value + 4, value + 5),
            dvec3(value + 6, value + 7, value + 8),
            value + 9,
            value + 10,
            value + 11,
            time};
}

class EventProcessorTests : public testing::Test
{
protected:
    EventProcessor& events = *EventProcessor::getInstance();
    RocketPhysicalProperties properties{};
    PhysicsSolver physics{properties, CelestialBodyType::planet, 1.0, 12};
    u64 runId{0};

    void SetUp() override
    {
        events.endCommandRun();
        events.clearCommandHistory();
        events.publishTelemetry({});
        events.terminatePythonMachine(false);
        properties.position = SolarSystemConstants::earthPos + dvec3(0, 2, 0);
        properties.velocity = dvec3(0);
        properties.mass = 10;
        physics.establishInitialOrientation(properties.position, 0, 0);
        physics.reset();
        runId = events.beginCommandRun();
    }

    void TearDown() override
    {
        events.endCommandRun();
        events.clearCommandHistory();
        events.publishTelemetry({});
        events.terminatePythonMachine(false);
    }
};

TEST_F(EventProcessorTests, CommandsAreDeferredAndOrderedEvenWhenTimeIsPaused)
{
    const auto initialThrust = physics.thrustMagnitude;
    events.setThrustMagnitude(runId, 0.2);
    events.changeThrustRotatation(runId, dvec3(10, 20, 30));
    events.enqueueCommand(runId, {RocketCommandCode::GYRO_ROTATION_CHANGE_X, 1});
    events.enqueueCommand(runId, {RocketCommandCode::GYRO_ROTATION_CHANGE_Y, 2});
    events.enqueueCommand(runId, {RocketCommandCode::GYRO_ROTATION_CHANGE_Z, 3});
    events.setThrustMagnitude(runId, 0.8);
    EXPECT_DOUBLE_EQ(physics.thrustMagnitude, initialThrust);
    EXPECT_EQ(properties.rotation, dvec3(0));
    EXPECT_TRUE(events.getCommandHistory().empty());

    events.applyCommands(physics, 0);
    EXPECT_DOUBLE_EQ(physics.thrustMagnitude, 0.8);
    EXPECT_EQ(properties.rotation, dvec3(11, 22, 33));

    const RocketCommand expected[] = {{RocketCommandCode::THRUST_MAGNITUDE_CHANGE, 0.2},
                                      {RocketCommandCode::THRUST_ROTATION_CHANGE_X, 10},
                                      {RocketCommandCode::THRUST_ROTATION_CHANGE_Y, 20},
                                      {RocketCommandCode::THRUST_ROTATION_CHANGE_Z, 30},
                                      {RocketCommandCode::GYRO_ROTATION_CHANGE_X, 1},
                                      {RocketCommandCode::GYRO_ROTATION_CHANGE_Y, 2},
                                      {RocketCommandCode::GYRO_ROTATION_CHANGE_Z, 3},
                                      {RocketCommandCode::THRUST_MAGNITUDE_CHANGE, 0.8}};
    ASSERT_EQ(events.getCommandHistory().size(), std::size(expected));
    size_t index = 0;
    for (const auto& [time, command] : events.getCommandHistory())
    {
        EXPECT_EQ(time, 0);
        EXPECT_EQ(command.code(), expected[index].code());
        EXPECT_DOUBLE_EQ(command.value(), expected[index++].value());
    }
    events.applyCommands(physics, 0);
    EXPECT_EQ(events.getCommandHistory().size(), std::size(expected));
}

TEST_F(EventProcessorTests, StopDiscardsPendingAndLateCommands)
{
    const auto initialThrust = physics.thrustMagnitude;
    events.setThrustMagnitude(runId, 0.2);
    events.endCommandRun();
    events.setThrustMagnitude(runId, 0.9);
    events.changeThrustRotatation(runId, dvec3(10));
    events.applyCommands(physics, 0);
    EXPECT_DOUBLE_EQ(physics.thrustMagnitude, initialThrust);
    EXPECT_EQ(properties.rotation, dvec3(0));
    EXPECT_TRUE(events.getCommandHistory().empty());
}

TEST_F(EventProcessorTests, NewRunDiscardsPendingCommandsAndRejectsOldIdentifiers)
{
    events.setThrustMagnitude(runId, 0.9);
    const auto newRun = events.beginCommandRun();
    ASSERT_NE(newRun, runId);
    events.setThrustMagnitude(runId, 0.8);
    events.setThrustMagnitude(0, 0.7);
    events.setThrustMagnitude(newRun, 0.3);
    events.applyCommands(physics, 0);
    EXPECT_DOUBLE_EQ(physics.thrustMagnitude, 0.3);
    ASSERT_EQ(events.getCommandHistory().size(), 1);
    EXPECT_DOUBLE_EQ(events.getCommandHistory().begin()->second.value(), 0.3);
}

TEST_F(EventProcessorTests, QueueLimitRejectsWholeRotationWithoutPartialChanges)
{
    for (size_t index = 0; index < EventProcessor::commandQueueCapacity - 2; ++index)
        events.setThrustMagnitude(runId, 0.2);
    EXPECT_THROW(events.changeThrustRotatation(runId, dvec3(10)), std::runtime_error);
    events.setThrustMagnitude(runId, 0.3);
    events.setThrustMagnitude(runId, 0.4);
    EXPECT_THROW(events.setThrustMagnitude(runId, 0.9), std::runtime_error);
    events.applyCommands(physics, 123);
    EXPECT_DOUBLE_EQ(physics.thrustMagnitude, 0.4);
    EXPECT_EQ(properties.rotation, dvec3(0));
    EXPECT_EQ(events.getCommandHistory().size(), EventProcessor::commandQueueCapacity);
    EXPECT_NO_THROW(events.setThrustMagnitude(runId, 0.5));
}

TEST_F(EventProcessorTests, ConcurrentProducerAndConsumerPreserveEveryCommandInOrder)
{
    constexpr size_t count = 2000;
    auto producer =
        std::async(std::launch::async,
                   [&]
                   {
                       for (size_t index = 0; index < count; ++index)
                           events.setThrustMagnitude(runId, static_cast<double>(index) / count);
                   });
    do
    {
        events.applyCommands(physics, 42);
    } while (producer.wait_for(0ms) != std::future_status::ready);
    producer.get();
    events.applyCommands(physics, 42);

    ASSERT_EQ(events.getCommandHistory().size(), count);
    size_t index = 0;
    for (const auto& [time, command] : events.getCommandHistory())
    {
        EXPECT_EQ(time, 42);
        EXPECT_DOUBLE_EQ(command.value(), static_cast<double>(index++) / count);
    }
}

TEST_F(EventProcessorTests, TelemetryRemainsConsistentDuringConcurrentPublication)
{
    events.publishTelemetry(snapshot(0));
    auto reader = std::async(
        std::launch::async,
        [&]
        {
            for (int index = 0; index < 10000; ++index)
            {
                const auto data = events.getTelemetry();
                const auto expected = snapshot(data.runningTime);
                if (data.position != expected.position || data.rotation != expected.rotation ||
                    data.velocity != expected.velocity || data.altitude != expected.altitude ||
                    data.thrustMagnitude != expected.thrustMagnitude || data.mass != expected.mass)
                    return false;
            }
            return true;
        });
    for (u64 time = 1; time <= 10000; ++time)
        events.publishTelemetry(snapshot(time));
    EXPECT_TRUE(reader.get());
}

TEST_F(EventProcessorTests, ResetRejectsCommandsFromWorkerStillFinishing)
{
    std::promise<void> sent;
    std::promise<void> release;
    auto gate = release.get_future().share();
    ofsim_simulation::ProgramRunner runner;
    runner.start(
        [&]
        {
            events.setThrustMagnitude(runId, 0.9);
            sent.set_value();
            gate.wait();
            events.setThrustMagnitude(runId, 0.8);
        },
        [] {});
    sent.get_future().wait();
    events.endCommandRun();
    runner.requestStop();
    EXPECT_FALSE(runner.poll());
    physics.reset();
    events.publishTelemetry({});
    const auto newRun = events.beginCommandRun();
    release.set_value();
    const auto deadline = std::chrono::steady_clock::now() + 3s;
    while (runner.active() && std::chrono::steady_clock::now() < deadline)
    {
        runner.poll();
        std::this_thread::yield();
    }
    ASSERT_FALSE(runner.active());
    EXPECT_FALSE(runner.error());
    events.setThrustMagnitude(newRun, 0.3);
    events.applyCommands(physics, 0);
    EXPECT_DOUBLE_EQ(physics.thrustMagnitude, 0.3);
    EXPECT_EQ(events.getTelemetry().runningTime, 0);
    EXPECT_EQ(events.getTelemetry().position, dvec3(0));
    ASSERT_EQ(events.getCommandHistory().size(), 1);
}

TEST_F(EventProcessorTests, PythonApiUsesSnapshotAndQueuesCommandsFromPythonThreads)
{
    events.publishTelemetry(snapshot(1));
    ofsim_python_integration::PythonMachine python;
    python.runPythonOrbitalProgram(R"(
import orbital_fs
import threading

data = orbital_fs.get_orbital_data()
assert data == dict(positionX=1., positionY=2., positionZ=3.,
                 rotationX=4., rotationY=5., rotationZ=6.,
                 velocityX=7., velocityY=8., velocityZ=9.,
                 altitude=10., thrustMagnitude=11., rocketMass=12., shouldStop=False)
assert orbital_fs.orbital_thrust_magnitude_change(0.4) == 0
worker = threading.Thread(target=lambda: orbital_fs.orbital_rocket_rotation_change([1., 2., 3.]))
worker.start()
worker.join()
assert orbital_fs.get_orbital_data() == data
)",
                                   runId);
    EXPECT_TRUE(events.getCommandHistory().empty());
    events.applyCommands(physics, 0);
    EXPECT_DOUBLE_EQ(physics.thrustMagnitude, 0.4);
    EXPECT_EQ(properties.rotation, dvec3(1, 2, 3));
    EXPECT_EQ(events.getCommandHistory().size(), 4);
}

TEST_F(EventProcessorTests, PythonQueueOverflowIsReportedAsProgramError)
{
    for (size_t index = 0; index < EventProcessor::commandQueueCapacity; ++index)
        events.setThrustMagnitude(runId, 0.2);
    ofsim_python_integration::PythonMachine python;
    EXPECT_THROW(python.runPythonOrbitalProgram(
                     "import orbital_fs\norbital_fs.orbital_thrust_magnitude_change(0.9)\n", runId),
                 std::runtime_error);
    EXPECT_FALSE(Py_IsInitialized());
}

TEST_F(EventProcessorTests, PythonStopFlagWorksWithoutTelemetryPublication)
{
    events.terminatePythonMachine(true);
    ofsim_python_integration::PythonMachine python;
    EXPECT_NO_THROW(python.runPythonOrbitalProgram(
        "import orbital_fs\nassert orbital_fs.get_orbital_data()['shouldStop'] is True\n", runId));
}

TEST_F(EventProcessorTests, VmCommandsAreQueuedAndTelemetryMemoryLayoutIsPreserved)
{
    events.publishTelemetry(snapshot(1));
    ofsim_vm::VMachine vm(events, runId);
    vm.translateSourceCode("fset f0, 0.6\nset r0, 1\ncmd r0, f0\nftc\nhalt\n");
    vm.start();
    EXPECT_TRUE(events.getCommandHistory().empty());
    const double expected[] = {4, 5, 6, 7, 8, 9, 1, 2, 3, 12, 11, 10, 1};
    for (size_t index = 0; index < std::size(expected); ++index)
        EXPECT_DOUBLE_EQ(vm.getMemory()->fetchDWord(65528 - index * 8), expected[index]);
    events.applyCommands(physics, 0);
    EXPECT_DOUBLE_EQ(physics.thrustMagnitude, 0.6);
    EXPECT_EQ(events.getCommandHistory().size(), 1);
}

TEST_F(EventProcessorTests, VmQueueOverflowIsReportedAsProgramError)
{
    for (size_t index = 0; index < EventProcessor::commandQueueCapacity; ++index)
        events.setThrustMagnitude(runId, 0.2);
    ofsim_vm::VMachine vm(events, runId);
    vm.translateSourceCode("fset f0, 0.9\nset r0, 1\ncmd r0, f0\nhalt\n");
    ofsim_simulation::ProgramRunner runner;
    runner.start([&] { vm.start(); }, [&] { vm.stop(); });
    const auto deadline = std::chrono::steady_clock::now() + 3s;
    while (runner.active() && std::chrono::steady_clock::now() < deadline)
    {
        runner.poll();
        std::this_thread::yield();
    }
    ASSERT_FALSE(runner.active());
    EXPECT_TRUE(runner.error());
    EXPECT_FALSE(vm.isStarted());
}
} // namespace
