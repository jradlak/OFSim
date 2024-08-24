#pragma once

#include <queue>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "SimulationEvent.h"

namespace ofsim_events
{    
    class EventDispatcher
    {
    public:  
        
        void sendGUIEvent(StateEvent event)
        {
            {
                std::lock_guard<std::mutex> lock(_mutex);
                inbox.push(event);
            }

            _cond_var.notify_one();
        }

        void sendSimulationEvent(ofsim_events::SimulationEvent* simulationEvent)
        {
            StateEvent event = unpackSimulationEvent(simulationEvent);
            sendGUIEvent(event);
        }

        void sendSMEvent(StateEvent event)
        {
            {
                std::lock_guard<std::mutex> lock(_mutex);
                outbox.push(event);
            }

            _cond_var.notify_one();
        }

        StateEvent recieveGUIEvent()
        {            
            auto endTime = std::chrono::system_clock::now() + std::chrono::milliseconds(200);

            std::unique_lock<std::mutex> lock(_mutex);

            _cond_var
                .wait_until(lock, endTime, [this]() { return !inbox.empty(); });
            
            if (inbox.empty())
            {
                return StateEvent::NONE;
            }

            auto event = inbox.front();
            inbox.pop();
            return event;
        }

        StateEvent recieveSMEvent()
        {   
            auto endTime = std::chrono::system_clock::now() + std::chrono::milliseconds(200);

            std::unique_lock<std::mutex> lock(_mutex);           

            _cond_var
                .wait_until(lock, endTime, [this]() { return !outbox.empty(); });

            if (outbox.empty())
            {
                return StateEvent::NONE;
            }

            auto event = outbox.front();
            outbox.pop();
            return event;
        }

    private:
        std::queue<StateEvent> inbox;
        std::queue<StateEvent> outbox;

        std::mutex _mutex;
        std::condition_variable _cond_var;

        StateEvent unpackSimulationEvent(SimulationEvent* simulationEvent) { return simulationEvent->action; } 
    };
}