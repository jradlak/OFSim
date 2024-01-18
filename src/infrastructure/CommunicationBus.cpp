#include "CommunicationBus.h"

using namespace ofsim_infrastructure;

void CommunicationBus::publishCommand(RocketCommand cmd)
{
	std::lock_guard<std::mutex> lock(mc);
	commands.push(cmd);
	cc.notify_one();
}

RocketCommand CommunicationBus::getCommad(unsigned long long runningTime)
{
	std::unique_lock<std::mutex> lock(mc);
	while (commands.empty())
	{
		cc.wait(lock);
	}

	RocketCommand cmd = commands.front();
	commands.pop();
	
	commandHistory.insert(std::pair<unsigned long long, RocketCommand>(runningTime, cmd));

	return cmd;
}