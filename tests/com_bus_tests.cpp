#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <vector>

#include "../src/infrastructure/CommunicationBus.h"

namespace com_bus_tests {
	using namespace com_bus;

	TEST(CommunicationBus, simple_single_threaded)
	{
		auto bus_data{ std::make_unique<Tbus_data>() };

		std::vector<int> some = {1, 2, 3, 4, 5};

		publish_command(*bus_data, RocketCommand{ 1, 11.13 });
		publish_command(*bus_data, RocketCommand{ 2, 12.13 });
		publish_command(*bus_data, RocketCommand{ 3, 13.13 });

		RocketCommand command1{ get_command(*bus_data, 123) };
		RocketCommand command2{ get_command(*bus_data, 223) };
		RocketCommand command3{ get_command(*bus_data, 323) };

		EXPECT_EQ(command1.code(), 1);
		EXPECT_EQ(command1.value(), 11.13);

		EXPECT_EQ(command2.code(), 2);
		EXPECT_EQ(command2.value(), 12.13);

		EXPECT_EQ(command3.code(), 3);
		EXPECT_EQ(command3.value(), 13.13);
	}
}