#include "ODDMA.h"


using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;


ODDMA::ODDMA(Rocket* _rocket, PhysicsEngine* _physics, VMachine* _vm)
{
	rocket = _rocket;
	physics = _physics;
	vm = _vm;
}

void ODDMA::start()
{
	threadsStarted = true;
	stateProducer();
	stateConsumer();
}

void ODDMA::stop()
{
	threadsStarted = false;
}

void ODDMA::publishState(RocketStatus state)
{
}

RocketStatus ODDMA::getLastState()
{
	return RocketStatus();
}

void ODDMA::sendCommand()
{
}

void ODDMA::stateProducer()
{
	while (threadsStarted)
	{
		glm::dvec3 position = rocket->getPosition();
		glm::dvec3 rotation = rocket->getRotation();
		glm::dvec3 velocity = rocket->getVelocity();
		double altitude = physics->getAltitude();
		double thrust = physics->getThrustMagnitude();
		double mass = rocket->getMass();

		RocketStatus status;
		status.timestamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		status.mass = mass;
		status.thrustMagnitude = thrust;
		status.altitude = altitude;
		status.velocity = velocity;
		status.rotation = rotation;
		status.position = position;

		while (statusSemaphore);
		statusSemaphore = true;
		qStatuses.push(status);
		statusSemaphore = false;	
 	}
}

void ODDMA::stateConsumer()
{
	while (threadsStarted)
	{
		while (statusSemaphore);
		statusSemaphore = true;
		RocketStatus status = qStatuses.front();
		qStatuses.pop();
		statusSemaphore = false;

		Memory* memory = vm->getMemory();
		int address = memory->size - 8;
		
		memory->storeDWord(address, status.rotation.z);
		address -= 8;
		memory->storeDWord(address, status.rotation.y);
		address -= 8;
		memory->storeDWord(address, status.rotation.x);
		address -= 8;

		memory->storeDWord(address, status.velocity.z);
		address -= 8;
		memory->storeDWord(address, status.velocity.y);
		address -= 8;
		memory->storeDWord(address, status.velocity.x);
		address -= 8;

		memory->storeDWord(address, status.position.z);
		address -= 8;
		memory->storeDWord(address, status.position.y);
		address -= 8;
		memory->storeDWord(address, status.position.x);
		address -= 8;

		memory->storeDWord(address, status.mass);
		address -= 8;
		memory->storeDWord(address, status.thrustMagnitude);
		address -= 8;
		memory->storeDWord(address, status.altitude);
		address -= 8;
		memory->storeDWord(address, status.timestamp);
		address -= 8;
	}
}

void ODDMA::commandListener()
{
}
