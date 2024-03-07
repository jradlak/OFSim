#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <mutex>

#include "../math_and_physics/MathTypes.h"
#include "../world/Rocket.h"
#include "../math_and_physics/PhysicsSolver.h"

namespace ofsim_events 
{
    enum class UserAction 
	{
		FILE_NEW,
		FILE_OPEN,

		PROGRAM_FILE_OPENED,
		PROGRAM_START_EXECUTION,
		PROGRAM_STOP_EXECUTION,

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
		
			void setRocketValue(u64 value) { rocketValue = value;}
			u64 getRocketValue() { return rocketValue; }

            EventProcessor(EventProcessor const&) = delete;
            void operator=(EventProcessor const&) = delete;

			void povideRocketAndPhysics(Rocket* _rocket, ofsim_math_and_physics::PhysicsSolver* _physics) { rocket = _rocket; physics = _physics; }

            static EventProcessor* getInstance();
			
        protected:
            EventProcessor() {};
            ~EventProcessor() {};

        private:			
			Rocket* rocket;
			ofsim_math_and_physics::PhysicsSolver* physics;

            UserEvent* userEvent = nullptr;
            u32 eventCounter {0};

            u64 currentTime();

			f64 rocketValue { 123.0 };

            static EventProcessor* instance;
            static std::mutex mutex;
    };
}