#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "../src/infrastructure/EventProcessor.h"
#include "../src/infrastructure/SMEventsListener.h"
#include "../src/infrastructure/StateMachine.h"

namespace state_machine_tests
{
	using namespace ofsim_events;

	TEST(StateMachineTest, basicTest)
	{
		std::unique_ptr<EventDispatcher> ed = std::make_unique<EventDispatcher>();
		std::unique_ptr<StateMachine> sm = std::make_unique<StateMachine>(*ed);
		std::unique_ptr<SMEventsListener> listener = std::make_unique<SMEventsListener>(*ed);

		ed->sendGUIEvent(StateEvent::START_SIMULATION);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		auto event = listener->lastEvent();
		int numberOfEvents = listener->numberOfEventsRecieved();

		EXPECT_EQ(event, StateEvent::SIMULATION_STARTED);
		EXPECT_EQ(numberOfEvents, 2);
	}
}