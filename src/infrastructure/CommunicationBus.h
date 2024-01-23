#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <map>
#include <chrono> 

#include "../math_and_physics/MathTypes.h"

#include "../vmachine/RocketCommand.h"


namespace ofsim_infrastructure
{
	class CommunicationBus
	{
	public:
		CommunicationBus() {}

		void publishCommand(RocketCommand cmd);
		RocketCommand getCommad(unsigned long long runningTime);

		std::map<u64, RocketCommand>& getCommandHistory() { return commandHistory; }

		bool anyCommands() { return commands.size() > 0; }

		void clear() { commandHistory.clear();}

		~CommunicationBus() {}

	private:
		std::queue<RocketCommand> commands;	

		mutable std::mutex mc;
		std::condition_variable cc;

		std::map<u64, RocketCommand> commandHistory;	
	};

}