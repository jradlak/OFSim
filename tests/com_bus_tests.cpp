#include <gtest/gtest.h>
#include <memory>


#include "../src/infrastructure/CommunicationBus.h"

using namespace com_bus;

TEST(CommunicationBus, test1)
{
	auto bus_data { std::make_unique<Tbus_data>() };
	
	publish_command(*bus_data, RocketCommand{ 1, 11.13 });
	publish_command(*bus_data, RocketCommand{ 2, 12.13 });
	publish_command(*bus_data, RocketCommand{ 3, 13.13 });

	RocketCommand command1 { get_command(*bus_data, 123) };
	RocketCommand command2 { get_command(*bus_data, 223) };
	RocketCommand command3 { get_command(*bus_data, 323) };
}