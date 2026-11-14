#pragma once

#include <string>
#include <mutex>
#include <map>
#include <atomic>
#include <vector>
#include <initializer_list>
#include <cstddef>

#include "RocketCommand.h"
#include "SimulationEvent.h"
#include "../math/MathTypes.h"
#include "../python_integration/PythonError.h"

namespace ofsim_math_and_physics
{
struct PhysicsSolver;
}

namespace ofsim_events
{
struct TelemetrySnapshot
{
    dvec3 position{0.0};
    dvec3 rotation{0.0};
    dvec3 velocity{0.0};
    f64 altitude{0.0};
    f64 thrustMagnitude{0.0};
    f64 mass{0.0};
    u64 runningTime{0};
};

// UI events, history and errors belong to the main thread. Workers exchange only
// queued commands, telemetry copies and the atomic Python stop flag.
class EventProcessor
{
public:
    SimulationEvent getEvent();
    void createEvent(StateEvent action, std::string data);

    TelemetrySnapshot getTelemetry();
    void publishTelemetry(const TelemetrySnapshot& snapshot);

    // Main-thread lifecycle: stop invalidates queued and subsequently sent commands.
    u64 beginCommandRun();
    void endCommandRun();
    void applyCommands(ofsim_math_and_physics::PhysicsSolver& physics, u64 runningTime);

    static constexpr std::size_t commandQueueCapacity = 4096;
    void enqueueCommand(u64 runId, RocketCommand command);
    void setThrustMagnitude(u64 runId, f64 thrust);
    void changeThrustRotatation(u64 runId, dvec3 deltaRotation);
    void processVMCommand(u64 runId, RocketCommand command);

    const std::multimap<u64, RocketCommand>& getCommandHistory() const
    {
        return command_history;
    }
    void clearCommandHistory()
    {
        command_history.clear();
    }

    void terminatePythonMachine(bool terminate)
    {
        shouldTerminatePythonMachine = terminate;
    }
    bool isPythonMachineTerminated()
    {
        return shouldTerminatePythonMachine;
    }
    void publishPythonError(ofsim_python_integration::PythonError error)
    {
        pythonError = error;
    }
    ofsim_python_integration::PythonError& getPythonError()
    {
        return pythonError;
    }

    static EventProcessor* getInstance();

    EventProcessor(EventProcessor const&) = delete;
    void operator=(EventProcessor const&) = delete;

protected:
    EventProcessor() {};
    ~EventProcessor() {};

private:
    void enqueueCommands(u64 runId, std::initializer_list<RocketCommand> commands);

    std::mutex exchangeMutex;
    TelemetrySnapshot telemetry;
    std::vector<RocketCommand> pendingCommands;
    u64 runCounter{0};
    u64 activeRun{0};

    SimulationEvent* userEvent = nullptr;
    u32 eventCounter{0};
    std::atomic<bool> shouldTerminatePythonMachine{false};
    ofsim_python_integration::PythonError pythonError;
    std::multimap<u64, RocketCommand> command_history;

    static EventProcessor* instance;
    static std::mutex mutex;
};
} // namespace ofsim_events
