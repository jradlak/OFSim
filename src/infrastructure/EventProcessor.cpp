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

u64 EventProcessor::currentTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();  
}


