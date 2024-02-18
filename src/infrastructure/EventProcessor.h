#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <mutex>

#include "../math_and_physics/MathTypes.h"

namespace ofsim_events 
{
    enum class UserAction 
	{
		FILE_NEW,
		FILE_OPEN,

		PROGRAM_FILE_OPENED,
		PROGRAM_START_EXECUTION,
		
		FILE_SAVE,	
		FILE_SAVED_AS,
		FILE_EXIT,

		VIEW_TELEMETRY,
		VIEW_TELEMETRY_PLOT,
		VIEW_COMMANDS,
		VIEW_PROGRAM,

		HELP_ABOUT,
		HELP_HELP,

		CHANGE_MODE_TO_FROM_PREDICTION,
		CHANGE_MODE_TO_FORM_PRESENTATION,

		NONE
	};

	struct UserEvent
	{		
		u32 id { 0 };
		u64 timestamp { 0 };
		UserAction action;
		std::string data;

		UserEvent() : action(UserAction::NONE) {}
		UserEvent(u32 _id, u64 _timestamp, UserAction _action, std::string _data)
			: id(_id), timestamp(_timestamp), action(_action), data(_data) {}
	};

    class EventProcessor
    {
        public:
            UserEvent getUserEvent();
            void createUserEvent(UserAction action, std::string data);

            EventProcessor(EventProcessor const&) = delete;
            void operator=(EventProcessor const&) = delete;

            static EventProcessor* getInstance();
        protected:
            EventProcessor() {};
            ~EventProcessor() {};

        private:
            UserEvent* userEvent = nullptr;
            u32 eventCounter {0};

            u64 currentTime();

            static EventProcessor* instance;
            static std::mutex mutex;
    };
}