#include "../CommunicationBus.h"

CommunicationBus::CommunicationBus()
{	
}

void CommunicationBus::publishCommand(RocketCommand cmd)
{
	std::lock_guard<std::mutex> lock(mc);
	commands.push(cmd);
	cc.notify_one();
}

RocketCommand CommunicationBus::getCommad(unsigned __int64 runningTime)
{
	std::unique_lock<std::mutex> lock(mc);
	while (commands.empty())
	{
		cc.wait(lock);
	}

	RocketCommand cmd = commands.front();
	commands.pop();
	
	commandHistory.insert(std::pair<unsigned __int64, RocketCommand>(runningTime, cmd));

	return cmd;
}

std::map<unsigned __int64, RocketCommand>& CommunicationBus::getCommandHistory()
{
	return commandHistory;
}

void CommunicationBus::clear()
{	
	commandHistory.clear();	
}
