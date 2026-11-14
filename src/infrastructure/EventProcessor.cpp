#include "EventProcessor.h"

#include <thread>
#include <chrono>
#include <stdexcept>

#include "../physics/PhysicsSolver.h"

using namespace ofsim_events;

EventProcessor* EventProcessor::instance = nullptr;
std::mutex EventProcessor::mutex;

TelemetrySnapshot EventProcessor::getTelemetry()
{
    std::lock_guard<std::mutex> lock(exchangeMutex);
    return telemetry;
}

void EventProcessor::publishTelemetry(const TelemetrySnapshot& snapshot)
{
    std::lock_guard<std::mutex> lock(exchangeMutex);
    telemetry = snapshot;
}

u64 EventProcessor::beginCommandRun()
{
    std::lock_guard<std::mutex> lock(exchangeMutex);
    pendingCommands.clear();
    activeRun = ++runCounter;
    return activeRun;
}

void EventProcessor::endCommandRun()
{
    std::lock_guard<std::mutex> lock(exchangeMutex);
    activeRun = 0;
    pendingCommands.clear();
}

void EventProcessor::enqueueCommands(u64 runId, std::initializer_list<RocketCommand> commands)
{
    std::lock_guard<std::mutex> lock(exchangeMutex);
    if (runId == 0 || runId != activeRun)
        return;
    if (commands.size() > commandQueueCapacity - pendingCommands.size())
        throw std::runtime_error("Flight command queue is full");
    pendingCommands.insert(pendingCommands.end(), commands.begin(), commands.end());
}

void EventProcessor::enqueueCommand(u64 runId, RocketCommand command)
{
    enqueueCommands(runId, {command});
}

void EventProcessor::processVMCommand(u64 runId, RocketCommand command)
{
    enqueueCommand(runId, command);
    // Preserve VM command pacing on the worker, never on the main thread.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void EventProcessor::applyCommands(ofsim_math_and_physics::PhysicsSolver& physics, u64 runningTime)
{
    std::vector<RocketCommand> commands;
    {
        std::lock_guard<std::mutex> lock(exchangeMutex);
        commands.swap(pendingCommands);
    }

    for (const auto& command : commands)
    {
        switch (command.code())
        {
        case RocketCommandCode::THRUST_MAGNITUDE_CHANGE:
            physics.updateThrustMagnitude(command.value());
            break;
        case RocketCommandCode::THRUST_ROTATION_CHANGE_X:
        case RocketCommandCode::GYRO_ROTATION_CHANGE_X:
            physics.rotateRocket(dvec3(command.value(), 0, 0));
            break;
        case RocketCommandCode::THRUST_ROTATION_CHANGE_Y:
        case RocketCommandCode::GYRO_ROTATION_CHANGE_Y:
            physics.rotateRocket(dvec3(0, command.value(), 0));
            break;
        case RocketCommandCode::THRUST_ROTATION_CHANGE_Z:
        case RocketCommandCode::GYRO_ROTATION_CHANGE_Z:
            physics.rotateRocket(dvec3(0, 0, command.value()));
            break;
        default:
            continue;
        }
        command_history.emplace(runningTime, command);
    }
}

EventProcessor* EventProcessor::getInstance()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr)
    {
        instance = new EventProcessor();
    }

    return instance;
}

SimulationEvent EventProcessor::getEvent()
{
    if (userEvent == nullptr)
    {
        return SimulationEvent();
    }

    SimulationEvent event =
        SimulationEvent(userEvent->id, userEvent->timestamp, userEvent->action, userEvent->data);
    delete userEvent;
    userEvent = nullptr;

    return event;
}

void EventProcessor::createEvent(StateEvent action, std::string data)
{
    if (userEvent == nullptr)
    {
        userEvent = new SimulationEvent(eventCounter++, getTelemetry().runningTime, action, data);
    }
}

void EventProcessor::setThrustMagnitude(u64 runId, f64 thrust)
{
    enqueueCommand(runId, {RocketCommandCode::THRUST_MAGNITUDE_CHANGE, thrust});
}

void EventProcessor::changeThrustRotatation(u64 runId, dvec3 deltaRotation)
{
    enqueueCommands(runId, {{RocketCommandCode::THRUST_ROTATION_CHANGE_X, deltaRotation.x},
                            {RocketCommandCode::THRUST_ROTATION_CHANGE_Y, deltaRotation.y},
                            {RocketCommandCode::THRUST_ROTATION_CHANGE_Z, deltaRotation.z}});
}
