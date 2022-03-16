#include "ODDMA.h"

ODDMA::ODDMA(Rocket* _rocket, VMachine* _vm)
{
	rocket = _rocket;
	vm = _vm;
}

void ODDMA::start()
{
}

void ODDMA::stop()
{
}

void ODDMA::publishState(RocketState state)
{
}

RocketState ODDMA::getLastState()
{
	return RocketState();
}

void ODDMA::sendCommand()
{
}

void ODDMA::stateProducer()
{
}

void ODDMA::stateConsumer()
{
}

void ODDMA::commandListener()
{
}
