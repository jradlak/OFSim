#pragma once

#include <string>
#include <chrono>
#include <ctime>
#include <mutex>
#include <map>

#include "RocketCommand.h"

#include "SimulationEvent.h"

#include "../math_and_physics/MathTypes.h"
#include "../world/Rocket.h"
#include "../math_and_physics/PhysicsSolver.h"
#include "../python_integration/PythonError.h"

namespace ofsim_events 
{   
	// Central singleton responsible for creating nad passing events and data between different parts of the application.	
    class EventProcessor
    {
        public:
			// UI events:
            SimulationEvent getEvent();
            void createEvent(StateEvent action, std::string data);

			// Rocket status:
            dvec3 getRocketPosition() { return rocket->properties().position; }
            dvec3 getRocketRotation() { return rocket->properties().rotation; }
            dvec3 getRocketVelocity() { return rocket->properties().velocity; }
            f64 getAltitude() { return physics->altitude; }
            f64 getThrustMagnitude() { return physics->thrustMagnitude; }
            f64 getRocketMass() { return rocket->properties().mass; }

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
			
			// VM integration:
			void processVMCommand(RocketCommand command);
			void provideRunnigTume(u64 _runntingTime) { runningTime = _runntingTime; }
			u64 getRunningTime() { return runningTime; }

			u64 currentTime();

			static EventProcessor* getInstance();
							
			// utility methods:
            void povideRocketAndPhysics(ofsim_world::Rocket* _rocket, ofsim_math_and_physics::PhysicsSolver* _physics) { rocket = _rocket; physics = _physics; }
			
            EventProcessor(EventProcessor const&) = delete;
            void operator=(EventProcessor const&) = delete;
            			
        protected:
            EventProcessor() {};
            ~EventProcessor() {};

        private:
			// since this class is a sigleton I don't figure it out how to pass references to 
			// objects instead of pointers,
			// because references should be intilized immediately
            ofsim_world::Rocket* rocket;
			ofsim_math_and_physics::PhysicsSolver* physics;
		
            SimulationEvent* userEvent = nullptr;
            u32 eventCounter {0};
			bool shouldTerminatePythonMachine { false };	
			ofsim_python_integration::PythonError pythonError;

			std::map<u64, RocketCommand> command_history;
           			
            static EventProcessor* instance;
            static std::mutex mutex;
	
			u64 runningTime { 0 };
    };
}
