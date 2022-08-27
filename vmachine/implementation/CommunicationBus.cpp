#include "../CommunicationBus.h"

unsigned __int64 getCurrentTime();

CommunicationBus::CommunicationBus()
{
	startTime = getCurrentTime();
}

void CommunicationBus::publishCommand(RocketCommand cmd)
{
	std::lock_guard<std::mutex> lock(mc);
	commands.push(cmd);
	cc.notify_one();
}

RocketCommand CommunicationBus::getCommad()
{
	std::unique_lock<std::mutex> lock(mc);
	while (commands.empty())
	{
		cc.wait(lock);
	}

	RocketCommand cmd = commands.front();
	commands.pop();
	
	commandHistory.insert(std::pair<unsigned __int64, RocketCommand>(getCurrentTime() - startTime, cmd));

	return cmd;
}

std::map<unsigned __int64, RocketCommand>& CommunicationBus::getCommandHistory()
{
	return commandHistory;
}

unsigned __int64 getCurrentTime()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
		).count();
}