#include "EventProcessor.h"

using namespace ofsim_events;

EventProcessor* EventProcessor::instance = nullptr;
std::mutex EventProcessor::mutex;

EventProcessor *EventProcessor::getInstance()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr)
    {
        instance = new EventProcessor();
    }

    return instance;
}

UserEvent EventProcessor::getUserEvent()
{
    if (userEvent == nullptr)
    {
         return UserEvent();
    }

    UserEvent event = UserEvent(userEvent->id, userEvent->timestamp, userEvent->action, userEvent->data);
    delete userEvent;
    userEvent = nullptr;

    return event;
}

void EventProcessor::createUserEvent(UserAction action, std::string data)
{
    if (userEvent == nullptr)
    {
        userEvent = new UserEvent(eventCounter++, currentTime(), action, data);
    }
}

void ofsim_events::EventProcessor::setThrustMagnitude(f64 thrust)
{
    physics->updateThrustMagnitude(thrust);
    RocketCommand command { RocketCommandCode::THRUST_MAGNITUDE_CHANGE, thrust };
    command_history.insert(std::pair<u64, RocketCommand>(currentTime(), command));
}

void ofsim_events::EventProcessor::changeThrustRotatation(dvec3 deltaRotation)
{
    physics->rotateRocket(deltaRotation);
    
    RocketCommand command1 { RocketCommandCode::THRUST_ROTATION_CHANGE_X, deltaRotation.x };
    command_history.insert(std::pair<u64, RocketCommand>(currentTime(), command1));
    
    RocketCommand command2 = RocketCommand(RocketCommandCode::THRUST_ROTATION_CHANGE_Y, deltaRotation.y);
    command_history.insert(std::pair<u64, RocketCommand>(currentTime() + 1, command2));
    
    RocketCommand command3 = RocketCommand(RocketCommandCode::THRUST_ROTATION_CHANGE_Z, deltaRotation.z);
    command_history.insert(std::pair<u64, RocketCommand>(currentTime() + 2, command3));
}

u64 EventProcessor::currentTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();  
}


