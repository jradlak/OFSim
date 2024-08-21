#pragma once

#include <queue>
#include <chrono>
#include <mutex>
#include <condition_variable>

namespace ofsim_infrastructure
{
    enum class StateEvent
    {
        NONE,

        // file dialog:
        OPEN_FILE_DIALOG,
        CLOSE_FILE_DIALOG,
        LOAD_DIRECTORY_FILES,
        LOAD_SELECTED_FILE,

        // simulation:
        START_SIMULATION,
        SIMULATION_STARTED,
        STOP_SIMULATION,
        PAUSE_SIMULATION,
        CONTINUE_SIMULATION,

        SWITCH_TO_TRAJECTORY_PREDICTION,
        SWITCH_TO_DIAGNOSTICS,
        SWITCH_TO_PRESENTATION,

        SHOW_INITIAL_CONFIGURATION,
        SHOW_HELP_DIALOG,
        SHOW_FILE_OPEN_DIALOG,
        SHOW_TRAJECTORY_PREDICTION
    };

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
    };
}