#include <gtest/gtest.h>

#include <future>
#include <thread>

#include "../src/python_integration/PythonMachine.h"
#include "../src/simulation/ProgramRunner.h"
#include "../src/vmachine/VMachine.h"

#ifdef __linux__
#include <filesystem>
#include <fstream>
#include <unistd.h>
#include "../src/physics/PhysicsSolver.h"
#endif

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
    vm.translateSourceCode("set r0, 65536\n bld r1, r0\n halt\n");
    ProgramRunner runner;
    runner.start([&] { vm.start(); }, [&] { vm.stop(); });
    ASSERT_TRUE(collect(runner));
    ASSERT_TRUE(runner.error());
    EXPECT_THROW(std::rethrow_exception(runner.error()), std::out_of_range);
    EXPECT_FALSE(vm.isStarted());
}

TEST(ProgramRunnerTests, InvalidVmRegistersReportErrorsAndAllowNextRun)
{
    ProgramRunner runner;
    for (const unsigned char index : {16, 17, 255})
    {
        ofsim_vm::VMachine vm(*ofsim_events::EventProcessor::getInstance());
        vm.translateSourceCode("set r0, 42\n halt\n");
        vm.getMemory()->storeByte(1, index);
        runner.start([&] { vm.start(); }, [&] { vm.stop(); });
        ASSERT_TRUE(collect(runner));
        ASSERT_TRUE(runner.error());
        EXPECT_FALSE(vm.isStarted());
        try
        {
            std::rethrow_exception(runner.error());
        }
        catch (const std::out_of_range& error)
        {
            EXPECT_NE(std::string(error.what()).find("VM register index out of range"),
                      std::string::npos);
        }

        ofsim_vm::VMachine nextVm(*ofsim_events::EventProcessor::getInstance());
        nextVm.translateSourceCode("set r0, 256\n set r1, 42\n bst r0, r1\n halt\n");
        runner.start([&] { nextVm.start(); }, [&] { nextVm.stop(); });
        ASSERT_TRUE(collect(runner));
        EXPECT_FALSE(runner.error());
        EXPECT_EQ(nextVm.getMemory()->fetchByte(256), 42);
    }
}

TEST(ProgramRunnerTests, OverflowingVmMemoryRangeReportsAnError)
{
    ofsim_vm::VMachine vm(*ofsim_events::EventProcessor::getInstance());
    vm.translateSourceCode("set r0, 0\n ld r1, r0\n halt\n");
    // Encode UINT_MAX - 3 without depending on the translator's integer parser.
    vm.getMemory()->storeWord(2, -4);
    ProgramRunner runner;
    runner.start([&] { vm.start(); }, [&] { vm.stop(); });
    ASSERT_TRUE(collect(runner));
    ASSERT_TRUE(runner.error());
    EXPECT_THROW(std::rethrow_exception(runner.error()), std::out_of_range);
    EXPECT_FALSE(vm.isStarted());
}

TEST(ProgramRunnerTests, DirectConditionalAndIndirectSelfJumpsStayActiveUntilStopped)
{
    for (const std::string jump : {"jmp loop", "jz loop", "jmpr r2"})
    {
        SCOPED_TRACE(jump);
        ofsim_vm::VMachine vm(*ofsim_events::EventProcessor::getInstance());
        vm.translateSourceCode("set r2, loop\nset r0, 256\nset r1, 7\nbst r0, r1\n"
                               "cmp r1, r1\nloop: " +
                               jump + "\nset r0, 257\nset r1, 99\nbst r0, r1\nhalt");
        ProgramRunner runner;
        runner.start([&] { vm.start(); }, [&] { vm.stop(); });
        const auto deadline = std::chrono::steady_clock::now() + 3s;
        while (vm.getMemory()->fetchByte(256) != 7 && std::chrono::steady_clock::now() < deadline)
            std::this_thread::sleep_for(1ms);
        EXPECT_EQ(vm.getMemory()->fetchByte(256), 7);
        std::this_thread::sleep_for(30ms);
        EXPECT_FALSE(runner.poll());
        EXPECT_EQ(vm.getMemory()->fetchByte(257), 0);
        runner.requestStop();
        ASSERT_TRUE(collect(runner));
        EXPECT_FALSE(runner.error());
    }
}

TEST(ProgramRunnerTests, MalformedVmInstructionErrorsAreCollectedBeforeAValidRerun)
{
    ProgramRunner runner;
    for (const unsigned char opcode : {0x02, 0x04})
    {
        ofsim_vm::VMachine vm(*ofsim_events::EventProcessor::getInstance());
        vm.translateSourceCode("halt");
        vm.getMemory()->storeByte(0, opcode);
        runner.start([&] { vm.start(); }, [&] { vm.stop(); });
        ASSERT_TRUE(collect(runner));
        ASSERT_TRUE(runner.error());
        EXPECT_FALSE(vm.isStarted());
        try
        {
            std::rethrow_exception(runner.error());
        }
        catch (const std::exception& error)
        {
            EXPECT_NE(std::string(error.what()).find("address 0"), std::string::npos);
        }
        vm.translateSourceCode("halt");
        runner.start([&] { vm.start(); }, [&] { vm.stop(); });
        ASSERT_TRUE(collect(runner));
        EXPECT_FALSE(runner.error());
    }
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

TEST(ProgramRunnerTests, VmArithmeticFaultsIncludeInstructionAddressesAndAllowValidReruns)
{
    ProgramRunner runner;
    const std::pair<std::string, std::string> cases[] = {
        {"set r0, 17\nset r1, 0\ndiv r0, r1", "VM div by zero at address 12"},
        {"set r0, 17\nset r1, 0\nmod r0, r1", "VM mod by zero at address 12"},
        {"fset f0, 17.5\nfset f1, 0.0\nfdiv f0, f1", "VM fdiv by zero at address 20"},
        {"fset f0, 17.5\nfset f1, -0.0\nfdiv f0, f1", "VM fdiv by zero at address 20"},
        {"set r0, 1\nset r1, 32\nvshl r0, r1",
         "VM vshl shift count out of range: 32 at address 12"},
        {"set r0, 1\nset r1, 33\nvshr r0, r1",
         "VM vshr shift count out of range: 33 at address 12"},
        {"set r0, 1\nset r1, -1\nvshl r0, r1",
         "VM vshl shift count out of range: 4294967295 at address 12"},
        {"set r0, 1\nset r1, -1\nvshr r0, r1",
         "VM vshr shift count out of range: 4294967295 at address 12"}};
    for (const auto& [source, message] : cases)
    {
        SCOPED_TRACE(source);
        ofsim_vm::VMachine vm(*ofsim_events::EventProcessor::getInstance());
        vm.translateSourceCode(source + "\nhalt");
        runner.start([&] { vm.start(); }, [&] { vm.stop(); });
        ASSERT_TRUE(collect(runner));
        ASSERT_TRUE(runner.error());
        EXPECT_FALSE(vm.isStarted());
        try
        {
            std::rethrow_exception(runner.error());
        }
        catch (const std::domain_error& error)
        {
            EXPECT_EQ(error.what(), message);
        }

        vm.translateSourceCode("set r0, 256\nset r1, 42\nbst r0, r1\nhalt");
        runner.start([&] { vm.start(); }, [&] { vm.stop(); });
        ASSERT_TRUE(collect(runner));
        EXPECT_FALSE(runner.error());
        EXPECT_EQ(vm.getMemory()->fetchByte(256), 42);
    }
}

TEST(ProgramRunnerTests, InvalidPythonApiCallsReportTypedErrorsAndAllowValidReruns)
{
    ProgramRunner runner;
    ofsim_python_integration::PythonMachine python;
    const std::pair<std::string, std::string> cases[] = {
        {"orbital_fs.orbital_thrust_magnitude_change()", "TypeError:"},
        {"orbital_fs.orbital_rocket_rotation_change([1, 2])", "ValueError: Rotation must contain"},
        {"orbital_fs.orbital_thrust_magnitude_change(10**10000)", "OverflowError:"},
        {"orbital_fs.orbital_rocket_rotation_change([1, 2, float('nan')])",
         "ValueError: Rotation values must be finite"},
        {"orbital_fs.get_orbital_data(1)", "TypeError:"}};
    for (const auto& [call, message] : cases)
    {
        SCOPED_TRACE(call);
        runner.start([&] { python.runPythonOrbitalProgram("import orbital_fs\n" + call); },
                     [&] { python.terminateProgram(); });
        ASSERT_TRUE(collect(runner));
        ASSERT_TRUE(runner.error());
        EXPECT_FALSE(Py_IsInitialized());
        try
        {
            std::rethrow_exception(runner.error());
        }
        catch (const std::runtime_error& error)
        {
            EXPECT_EQ(std::string(error.what()).find(message), 0);
        }

        runner.start(
            [&]
            {
                python.runPythonOrbitalProgram(
                    "import orbital_fs\nassert orbital_fs.orbital_thrust_magnitude_change(0.2) == "
                    "0\n"
                    "assert orbital_fs.orbital_rocket_rotation_change((1, 2, 3)) == 0\n"
                    "assert isinstance(orbital_fs.get_orbital_data(), dict)\n");
            },
            [&] { python.terminateProgram(); });
        ASSERT_TRUE(collect(runner));
        EXPECT_FALSE(runner.error());
        EXPECT_FALSE(Py_IsInitialized());
    }
}

TEST(ProgramRunnerTests, PythonStopRequestSetsFlagAndCollectsAfterScriptReturns)
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

#ifdef __linux__
TEST(ProgramRunnerTests, PythonRunsInAnotherProcessAndPreservesOutputAndThreadedApiCalls)
{
    auto& events = *ofsim_events::EventProcessor::getInstance();
    events.terminatePythonMachine(false);
    ofsim_python_integration::PythonMachine python;
    EXPECT_NO_THROW(python.runPythonOrbitalProgram(
        "import os, sys, threading, orbital_fs\nassert os.getpid() != " + std::to_string(getpid()) +
        R"(
print('worker stdout remains separate from IPC', flush=True)
print('worker stderr remains separate from IPC', file=sys.stderr, flush=True)
errors = []
def read_telemetry():
    try:
        for _ in range(50):
            assert isinstance(orbital_fs.get_orbital_data()['altitude'], float)
    except BaseException as error:
        errors.append(error)
threads = [threading.Thread(target=read_telemetry) for _ in range(4)]
for thread in threads:
    thread.start()
for thread in threads:
    thread.join()
assert not errors, errors
)"));
    EXPECT_NO_THROW(python.runPythonOrbitalProgram("#" + std::string(1024 * 1024, 'x') + "\npass"));
    EXPECT_FALSE(Py_IsInitialized());
}

TEST(ProgramRunnerTests, PythonLaunchAndProcessFailuresAllowValidReruns)
{
    auto& events = *ofsim_events::EventProcessor::getInstance();
    events.terminatePythonMachine(false);
    ofsim_python_integration::PythonMachine missing("/nonexistent/ofsim-python-worker");
    EXPECT_THROW(missing.runPythonOrbitalProgram("pass"), std::system_error);
    ofsim_python_integration::PythonMachine python;
    for (const std::string source :
         {"import os; os._exit(0)", "import os, signal; os.kill(os.getpid(), signal.SIGKILL)"})
    {
        SCOPED_TRACE(source);
        EXPECT_THROW(python.runPythonOrbitalProgram(source), std::runtime_error);
        EXPECT_NO_THROW(python.runPythonOrbitalProgram("assert 2 + 2 == 4"));
    }
    EXPECT_FALSE(Py_IsInitialized());
}

TEST(ProgramRunnerTests, PythonStartupTimeoutCollectsAnUnresponsiveWorker)
{
    const auto path = std::filesystem::temp_directory_path() /
                      ("ofsim-stalled-worker-" + std::to_string(getpid()));
    struct RemoveFile
    {
        std::filesystem::path path;
        ~RemoveFile()
        {
            std::error_code error;
            std::filesystem::remove(path, error);
        }
    } cleanup{path};
    std::ofstream(path) << "#!/bin/sh\nexec sleep 60\n";
    std::filesystem::permissions(path, std::filesystem::perms::owner_all);
    ofsim_events::EventProcessor::getInstance()->terminatePythonMachine(false);
    ofsim_python_integration::PythonMachine python(path.string());
    const auto started = std::chrono::steady_clock::now();
    try
    {
        python.runPythonOrbitalProgram("pass");
        FAIL() << "An unresponsive worker was accepted";
    }
    catch (const std::runtime_error& error)
    {
        EXPECT_NE(std::string(error.what()).find("startup timed out"), std::string::npos);
    }
    EXPECT_LT(std::chrono::steady_clock::now() - started, 7s);
}

TEST(ProgramRunnerTests, PythonStopBoundsLoopsSleepAndInterpreterFinalization)
{
    auto& events = *ofsim_events::EventProcessor::getInstance();
    for (const std::string body :
         {"while True: pass\n", "import time; time.sleep(60)\n",
          "import os\nos.setpgid(0, os.getpgid(os.getppid()))\nwhile True: pass\n",
          "import threading, time\n"
          "threading.Thread(target=lambda: time.sleep(60)).start()\n"})
    {
        SCOPED_TRACE(body);
        events.terminatePythonMachine(false);
        const auto runId = events.beginCommandRun();
        ofsim_world::RocketPhysicalProperties properties{};
        properties.position = dvec3(0, 100000, 0);
        properties.mass = 10;
        ofsim_math_and_physics::PhysicsSolver physics(properties, ofsim_world::planet, 6371, 12);
        ofsim_python_integration::PythonMachine python;
        ProgramRunner runner;
        runner.start(
            [&]
            {
                python.runPythonOrbitalProgram(
                    "import orbital_fs\norbital_fs.orbital_thrust_magnitude_change(0.5)\n" + body,
                    runId);
            },
            [&] { python.terminateProgram(); });
        const auto started = std::chrono::steady_clock::now();
        while (properties.thrustMagnitude != 0.5 && std::chrono::steady_clock::now() - started < 3s)
        {
            events.applyCommands(physics, 0);
            std::this_thread::sleep_for(1ms);
        }
        ASSERT_DOUBLE_EQ(properties.thrustMagnitude, 0.5);
        EXPECT_FALSE(runner.poll());
        events.endCommandRun();
        const auto stopping = std::chrono::steady_clock::now();
        runner.requestStop();
        EXPECT_FALSE(runner.poll());
        ASSERT_TRUE(collect(runner));
        EXPECT_LT(std::chrono::steady_clock::now() - stopping, 2s);
        EXPECT_FALSE(runner.error());
        events.terminatePythonMachine(false);
        EXPECT_NO_THROW(python.runPythonOrbitalProgram("pass"));
    }
    events.clearCommandHistory();
}

TEST(ProgramRunnerTests, PythonCooperativeStopRejectsLateCommandsAndImmediateStopsArePreserved)
{
    auto& events = *ofsim_events::EventProcessor::getInstance();
    ofsim_python_integration::PythonMachine python;
    events.terminatePythonMachine(false);
    const auto runId = events.beginCommandRun();
    ProgramRunner runner;
    runner.start(
        [&]
        {
            python.runPythonOrbitalProgram(R"(
import orbital_fs
while not orbital_fs.get_orbital_data()['shouldStop']:
    pass
orbital_fs.orbital_thrust_magnitude_change(0.8)
)",
                                           runId);
        },
        [&] { python.terminateProgram(); });
    events.endCommandRun();
    const auto stopping = std::chrono::steady_clock::now();
    runner.requestStop();
    ASSERT_TRUE(collect(runner));
    EXPECT_LT(std::chrono::steady_clock::now() - stopping, 900ms);
    EXPECT_FALSE(runner.error());

    ofsim_world::RocketPhysicalProperties properties{};
    properties.mass = 10;
    properties.position = dvec3(0, 100000, 0);
    ofsim_math_and_physics::PhysicsSolver physics(properties, ofsim_world::planet, 6371, 12);
    events.applyCommands(physics, 0);
    EXPECT_NE(properties.thrustMagnitude, 0.8);

    for (int iteration = 0; iteration < 3; ++iteration)
    {
        events.terminatePythonMachine(false);
        runner.start([&] { python.runPythonOrbitalProgram("while True: pass"); },
                     [&] { python.terminateProgram(); });
        runner.requestStop();
        ASSERT_TRUE(collect(runner));
        EXPECT_FALSE(runner.error());
    }
    events.terminatePythonMachine(false);
}
#endif
} // namespace
