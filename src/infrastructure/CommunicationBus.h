#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <map>
#include <chrono> 

#include "../vmachine/RocketCommand.h"

namespace ofs_infrastructure
{
	class CommunicationBus
	{
	public:
		CommunicationBus() {}

		void publishCommand(RocketCommand cmd);
		RocketCommand getCommad(unsigned long long runningTime);

		std::map<unsigned long long, RocketCommand>& getCommandHistory() { return commandHistory; }

		bool anyCommands() { return commands.size() > 0; }

		void clear() { commandHistory.clear();}

		~CommunicationBus() {}

	private:
		std::queue<RocketCommand> commands;	

		mutable std::mutex mc;
		std::condition_variable cc;

		std::map<unsigned long long, RocketCommand> commandHistory;	
	};

}