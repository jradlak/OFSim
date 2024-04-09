#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <mutex>
#include <map>

#include "RocketCommand.h"

#include "../math_and_physics/MathTypes.h"
#include "../world/Rocket.h"
#include "../math_and_physics/PhysicsSolver.h"
#include "../python_integration/PythonError.h"

namespace ofsim_events 
{
    enum class UserAction 
	{
		FILE_NEW,
		FILE_OPEN,

		PROGRAM_FILE_OPENED,
		PROGRAM_TRANSLATE,		
		PROGRAM_RAISE_ERROR,
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

	// This kind of event is producend by the user interface or by te Ptyhon machine when scriupt error occurs.
	struct SimulationEvent
	{		
		u32 id { 0 };
		u64 timestamp { 0 };
		UserAction action;
		std::string data;

		SimulationEvent() : action(UserAction::NONE) {}
		SimulationEvent(u32 _id, u64 _timestamp, UserAction _action, std::string _data)
			: id(_id), timestamp(_timestamp), action(_action), data(_data) {}
	};

	// Central singleton responsible for creating nad passing events and data between different parts of the application.	
    class EventProcessor
    {
        public:
			// UI events:
            SimulationEvent getEvent();
            void createEvent(UserAction action, std::string data);
		
			// Rocket status:
			dvec3 getRocketPosition() { return rocket->getPosition(); }
			dvec3 getRocketRotation() { return rocket->getRotation(); }
			dvec3 getRocketVelocity() { return rocket->getVelocity(); }
			f64 getAltitude() { return physics->getAltitude(); }
			f64 getThrustMagnitude() { return physics->getThrustMagnitude(); }
			f64 getRocketMass() { return rocket->getMass(); }

			// Rocket commands:
			void setThrustMagnitude(f64 thrust);
			void changeThrustRotatation(dvec3 deltaRotation);
			std::map<u64, RocketCommand> &getCommandHistory() { return command_history; }
			void clearCommandHistory() { command_history.clear(); }

			// Python integration:
			void terminatePythonMachine(bool terminate) { shouldTerminatePythonMachine = terminate;}			
			bool isPythonMachineTerminated() { return shouldTerminatePythonMachine; }
			void publishPythonError(ofsim_python_integration::PythonError error) { pythonError = error; }
			ofsim_python_integration::PythonError& getPythonError() { return pythonError; }

			// utility methods:
			void povideRocketAndPhysics(Rocket* _rocket, ofsim_math_and_physics::PhysicsSolver* _physics) { rocket = _rocket; physics = _physics; }
            EventProcessor(EventProcessor const&) = delete;
            void operator=(EventProcessor const&) = delete;
            static EventProcessor* getInstance();
			
        protected:
            EventProcessor() {};
            ~EventProcessor() {};

        private:			
			Rocket* rocket;
			ofsim_math_and_physics::PhysicsSolver* physics;

            SimulationEvent* userEvent = nullptr;
            u32 eventCounter {0};
			bool shouldTerminatePythonMachine { false };	
			ofsim_python_integration::PythonError pythonError;

			std::map<u64, RocketCommand> command_history;

            u64 currentTime();
			
            static EventProcessor* instance;
            static std::mutex mutex;
    };
}