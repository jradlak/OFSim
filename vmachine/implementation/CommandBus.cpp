#include "../CommandBus.h"

void CommandBus::publishCommand(RocketCommand cmd)
{
	std::lock_guard<std::mutex> lock(m);
	commands.push(cmd);
	c.notify_one();
}

RocketCommand CommandBus::getCommad()
{
	std::unique_lock<std::mutex> lock(m);
	while (commands.empty())
	{
		c.wait(lock);
	}

	RocketCommand cmd = commands.front();
	commands.pop();
	
	return cmd;
}
