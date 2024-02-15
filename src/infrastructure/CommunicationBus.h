#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <map>
#include <chrono> 

#include "../math_and_physics/MathTypes.h"
#include "RocketCommand.h"

// Communication bus
// This is a thread safe queue of commands
namespace com_bus
{
	struct Tgui_event
	{
		std::string event;
		std::string data;			
	};

	struct Tbus_data
	{
		std::queue<RocketCommand> commands;
		std::map<u64, RocketCommand> command_history;
		
		std::queue<Tgui_event> gui_events;
		
		mutable std::mutex mc;
		std::condition_variable cc;		
	};
			
	void publish_command(Tbus_data& bus, const RocketCommand cmd);
	RocketCommand get_command(Tbus_data& bus, const u64 runningTime);

	void send_gui_event(Tbus_data& bus, const Tgui_event event);

	bool any_commands(Tbus_data& bus);
	void clear_command_history(Tbus_data& bus);
}