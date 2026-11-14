#include <gtest/gtest.h>

#include <future>
#include <thread>

#include "../src/python_integration/PythonMachine.h"
#include "../src/simulation/ProgramRunner.h"
#include "../src/vmachine/VMachine.h"

namespace
{
using namespace std::chrono_literals;
using ofsim_simulation::ProgramRunner;

bool collect(ProgramRunner& runner)
{
    const auto deadline = std::chrono::steady_clock::now() + 3s;
    while (std::chrono::steady_clock::now() < deadline)
    {
        if (runner.poll())
            return true;
        std::this_thread::sleep_for(1ms);
    }
    return false;
}

TEST(ProgramRunnerTests, StopKeepsWorkerAndWorldAliveUntilCompletion)
{
    std::promise<void> release;
    auto gate = release.get_future().share();
    int stopRequests = 0;
    int worldValue = 0;
    ProgramRunner runner;
    runner.start(
        [&]
        {
            gate.wait();
            worldValue = 42; // A final access to objects that reset must not destroy.
        },
        [&] { ++stopRequests; });

    runner.requestStop();
    runner.requestStop();
    EXPECT_EQ(stopRequests, 1);
    EXPECT_TRUE(runner.active());
    EXPECT_FALSE(runner.poll()); // Must return even though the worker is blocked.
    EXPECT_THROW(runner.start([] {}, [] {}), std::logic_error);

    release.set_value();
    ASSERT_TRUE(collect(runner));
    EXPECT_FALSE(runner.active());
    EXPECT_EQ(worldValue, 42);
    EXPECT_FALSE(runner.error());
}

TEST(ProgramRunnerTests, ExceptionsAreCollectedAndNextRunCanStart)
{
    ProgramRunner runner;
    runner.start([] { throw std::runtime_error("program failed"); }, [] {});
    ASSERT_TRUE(collect(runner));
    ASSERT_TRUE(runner.error());
    EXPECT_THROW(std::rethrow_exception(runner.error()), std::runtime_error);

    runner.start([] {}, [] {});
    ASSERT_TRUE(collect(runner));
    EXPECT_FALSE(runner.error());
}

TEST(ProgramRunnerTests, DestructionRequestsStopBeforeReleasingWorker)
{
    std::promise<void> release;
    auto gate = release.get_future().share();
    bool finished = false;
    {
        ProgramRunner runner;
        runner.start(
            [&]
            {
                gate.wait();
                finished = true;
            },
            [&] { release.set_value(); });
    }
    EXPECT_TRUE(finished);
}

TEST(ProgramRunnerTests, PollDoesNotJoinWhileThreadIsStillCleaningUp)
{
    struct ExitGate
    {
        std::shared_future<void> release;
        ~ExitGate()
        {
            release.wait();
        }
    };
    std::promise<void> release;
    auto gate = release.get_future().share();
    std::promise<void> workReturned;
    ProgramRunner runner;
    runner.start(
        [&]
        {
            thread_local ExitGate cleanup{gate};
            workReturned.set_value();
        },
        [&] { release.set_value(); });

    workReturned.get_future().wait();
    EXPECT_FALSE(runner.poll());
    runner.requestStop();
    ASSERT_TRUE(collect(runner));
}

TEST(ProgramRunnerTests, ImmediateStopAndRepeatedVmRuns)
{
    auto& events = *ofsim_events::EventProcessor::getInstance();
    for (int iteration = 0; iteration < 20; ++iteration)
    {
        ofsim_vm::VMachine vm(events);
        ProgramRunner runner;
        vm.translateSourceCode("loop:\n set r0, 1\n jmp loop\n");
        const auto firstByte = vm.getMemory()->fetchByte(0);
        runner.start([&] { vm.start(); }, [&] { vm.stop(); });
        runner.requestStop();
        ASSERT_TRUE(collect(runner));
        EXPECT_FALSE(vm.isStarted());
        EXPECT_FALSE(runner.error());
        EXPECT_EQ(vm.getMemory()->fetchByte(0), firstByte);
    }
}

TEST(ProgramRunnerTests, VmStopBeforeWorkerStartsIsNotLost)
{
    ofsim_vm::VMachine vm(*ofsim_events::EventProcessor::getInstance());
    // This would overwrite byte 256 if start() discarded the pending stop.
    vm.translateSourceCode("set r0, 256\n set r1, 42\n bst r0, r1\n halt\n");
    vm.stop();
    vm.start();
    EXPECT_EQ(vm.getMemory()->fetchByte(256), 0);
    EXPECT_NE(vm.getMemory()->fetchByte(0), 0);
    EXPECT_FALSE(vm.isStarted());
}

TEST(ProgramRunnerTests, RunningVmCanBeStoppedWithoutClearingItsMemory)
{
    ofsim_vm::VMachine vm(*ofsim_events::EventProcessor::getInstance());
    vm.translateSourceCode("loop:\n set r0, 256\n set r1, 42\n bst r0, r1\n jmp loop\n");
    ProgramRunner runner;
    runner.start([&] { vm.start(); }, [&] { vm.stop(); });

    const auto deadline = std::chrono::steady_clock::now() + 3s;
    while (vm.getMemory()->fetchByte(256) != 42 && std::chrono::steady_clock::now() < deadline)
    {
        std::this_thread::sleep_for(1ms);
    }
    runner.requestStop();
    ASSERT_TRUE(collect(runner));
    EXPECT_FALSE(runner.error());
    EXPECT_EQ(vm.getMemory()->fetchByte(256), 42);
}

TEST(ProgramRunnerTests, VmExecutionExceptionDoesNotEscapeWorker)
{
    ofsim_vm::VMachine vm(*ofsim_events::EventProcessor::getInstance());
    vm.translateSourceCode("set r0, 70000\n bld r1, r0\n halt\n");
    ProgramRunner runner;
    runner.start([&] { vm.start(); }, [&] { vm.stop(); });
    ASSERT_TRUE(collect(runner));
    EXPECT_TRUE(runner.error());
    EXPECT_FALSE(vm.isStarted());
}

TEST(ProgramRunnerTests, PythonErrorsAndSubsequentRunsAreCollectedAfterFinalization)
{
    ofsim_python_integration::PythonMachine python;
    ProgramRunner runner;
    for (int iteration = 0; iteration < 3; ++iteration)
    {
        runner.start(
            [&]
            {
                python.runPythonOrbitalProgram(
                    "import orbital_fs\nraise RuntimeError('autopilot failed')\n");
            },
            [&] { python.terminateProgram(); });
        ASSERT_TRUE(collect(runner));
        ASSERT_TRUE(runner.error());
        try
        {
            std::rethrow_exception(runner.error());
        }
        catch (const std::runtime_error& error)
        {
            EXPECT_NE(std::string(error.what()).find("autopilot failed"), std::string::npos);
        }
        EXPECT_FALSE(Py_IsInitialized());

        runner.start(
            [&]
            {
                python.runPythonOrbitalProgram(
                    "import orbital_fs\nvalue = sum(range(10))\nassert value == 45\n");
            },
            [&] { python.terminateProgram(); });
        ASSERT_TRUE(collect(runner));
        EXPECT_FALSE(runner.error());
        EXPECT_FALSE(Py_IsInitialized());
    }
}

TEST(ProgramRunnerTests, StoppingPythonDoesNotWaitForItsSleep)
{
    ofsim_python_integration::PythonMachine python;
    auto& events = *ofsim_events::EventProcessor::getInstance();
    events.terminatePythonMachine(false);
    ProgramRunner runner;
    runner.start([&] { python.runPythonOrbitalProgram("import time\ntime.sleep(0.1)\n"); },
                 [&] { python.terminateProgram(); });
    runner.requestStop();
    EXPECT_TRUE(events.isPythonMachineTerminated());
    ASSERT_TRUE(collect(runner));
    EXPECT_FALSE(runner.error());
    EXPECT_FALSE(Py_IsInitialized());
    events.terminatePythonMachine(false);
}
} // namespace
