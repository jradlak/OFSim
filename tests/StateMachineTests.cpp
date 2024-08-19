#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "../src/infrastructure/EventProcessor.h"
#include "../src/infrastructure/SMEventsListener.h"
#include "../src/infrastructure/StateMachine.h"

namespace state_machine_tests
{
	using namespace ofsim_infrastructure;

	TEST(StateMachineTest, basicTest)
	{
		EventDispatcher ed;
		StateMachine sm(ed);
		SMEventsListener listener(ed);

		ed.sendGUIEvent(StateEvent::START_SIMULATION);

		std::this_thread::sleep_for(std::chrono::milliseconds(200));

		auto event = listener.lastEvent();
		int numberOfEvents = listener.numberOfEventsRecieved();

		EXPECT_EQ(event, StateEvent::SIMULATION_STARTED);		
		EXPECT_EQ(numberOfEvents, 2);
	}
}