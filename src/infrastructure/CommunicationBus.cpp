#include "CommunicationBus.h"

using namespace ofsim_infrastructure;

void CommunicationBus::publishCommand(RocketCommand cmd)
{
	std::lock_guard<std::mutex> lock(mc);
	commands.push(cmd);
	cc.notify_one();
}

RocketCommand CommunicationBus::getCommad(u64 runningTime)
{
	std::unique_lock<std::mutex> lock(mc);
	while (commands.empty())
	{
		cc.wait(lock);
	}

	RocketCommand cmd = commands.front();
	commands.pop();
	
	commandHistory.insert(std::pair<u64, RocketCommand>(runningTime, cmd));

	return cmd;
}