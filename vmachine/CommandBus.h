#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

#include "RocketCommand.h"

class CommandBus
{
public:
	CommandBus() {}

	void publishCommand(RocketCommand cmd);

	RocketCommand getCommad();

	~CommandBus() {}

private:
	std::queue<RocketCommand> commands;	

	mutable std::mutex m;
	std::condition_variable c;
};