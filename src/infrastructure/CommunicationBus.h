#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <map>
#include <chrono> 

#include "../math_and_physics/MathTypes.h"
#include "../vmachine/RocketCommand.h"

// Communication bus
// This is a thread safe queue of commands
namespace com_bus
{
	struct Tbus_data
	{
		std::queue<RocketCommand> commands;
		std::map<u64, RocketCommand> command_history;

		mutable std::mutex mc;
		std::condition_variable cc;		
	};
			
	void publish_command(Tbus_data& bus, const RocketCommand cmd);
	RocketCommand get_command(Tbus_data& bus, const u64 runningTime);

	bool any_commands(Tbus_data& bus);
	void clear_command_history(Tbus_data& bus);
}