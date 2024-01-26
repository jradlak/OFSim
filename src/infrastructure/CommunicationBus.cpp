#include "CommunicationBus.h"

namespace com_bus {
	void publish_command(Tbus_data& bus, RocketCommand cmd)
	{
		std::lock_guard<std::mutex> lock(bus.mc);
		bus.commands.push(cmd);
		bus.cc.notify_one();
	}

	RocketCommand get_command(Tbus_data& bus, const u64 runningTime)
	{
		std::unique_lock<std::mutex> lock(bus.mc);
		while (bus.commands.empty())
		{
			bus.cc.wait(lock);
		}

		RocketCommand cmd = bus.commands.front();
		bus.commands.pop();

		bus.command_history.insert(std::pair<u64, RocketCommand>(runningTime, cmd));

		return cmd;
	}

	bool any_commands(Tbus_data& bus) { return bus.commands.size() > 0; }
	void clear_command_history(Tbus_data& bus) { bus.command_history.clear(); }
}