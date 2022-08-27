#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <map>
#include <chrono> 

#include "RocketCommand.h"

class CommunicationBus
{
public:
	CommunicationBus();

	void publishCommand(RocketCommand cmd);
	RocketCommand getCommad();

	std::map<unsigned __int64, RocketCommand>& getCommandHistory();

	~CommunicationBus() {}

private:
	std::queue<RocketCommand> commands;	
	std::queue<std::string> messages;

	mutable std::mutex mc;
	std::condition_variable cc;

	std::map<unsigned __int64, RocketCommand> commandHistory;

	unsigned __int64 startTime = 0;
};