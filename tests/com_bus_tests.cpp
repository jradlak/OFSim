#include <gtest/gtest.h>
#include <memory>
#include <thread>

#include "../src/infrastructure/CommunicationBus.h"

namespace com_bus_tests {
	using namespace com_bus;

	TEST(CommunicationBus, simple_single_threaded)
	{
		auto bus_data{ std::make_unique<Tbus_data>() };

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

	TEST(CommunicationBus, simple_multi_threaded)
	{
		auto bus_data{ std::make_unique<Tbus_data>() };

		std::thread t1{ publish_command, std::ref(*bus_data), RocketCommand{ 1, 11.13 } };
		std::thread t2{ publish_command, std::ref(*bus_data), RocketCommand{ 2, 12.13 } };
		std::thread t3{ publish_command, std::ref(*bus_data), RocketCommand{ 3, 13.13 } };

		t1.join();
		t2.join();
		t3.join();

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

	TEST(CommunicationBus, simple_multi_threaded_2)
	{
		auto bus_data{ std::make_unique<Tbus_data>() };

		std::thread t1{ publish_command, std::ref(*bus_data), RocketCommand{ 1, 11.13 } };
		std::thread t2{ publish_command, std::ref(*bus_data), RocketCommand{ 2, 12.13 } };
		std::thread t3{ publish_command, std::ref(*bus_data), RocketCommand{ 3, 13.13 } };

		t1.join();
		t2.join();
		t3.join();

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

	TEST(CommunicationBus, produces_consumer_test)
	{
		auto bus_data{ std::make_unique<Tbus_data>() };
		bool producer_finished{ false };
		bool consumer_finished{ false };
		int produced_comands{ 0 }, consumed_commands{ 0 };

		auto producer { std::thread{ [&bus_data, &producer_finished, &produced_comands]() {
			while (!producer_finished)
			{
				int id = (int)rand();
				publish_command(*bus_data, RocketCommand{ id, double(rand() / 100.0) });
				if (id < 10)
				{
					producer_finished = true;
				}

				produced_comands++;
			}			
		}}};

		auto consumer { std::thread{ [&bus_data, &consumer_finished, &consumed_commands]() {
			while (!consumer_finished)
			{
				RocketCommand command{ get_command(*bus_data, 123) };
				if (command.code() < 10)
				{
					consumer_finished = true;					
				}

				consumed_commands++;
			}
		}}};	

		producer.join();
		consumer.join();
		
		EXPECT_EQ(produced_comands, consumed_commands);
	}
}