#include "ODDMA.h"

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;

ODDMA::ODDMA(Rocket* _rocket, ofsim_math_and_physics::PhysicsSolver* _physics, VMachine* _vm, ofsim_infrastructure::CommunicationBus* _commandBus)
{
	rocket = _rocket;
	physics = _physics;
	vm = _vm;
	commandBus = _commandBus;
}

void ODDMA::start()
{
	threadsStarted = true;

	std::thread stateProducerThread = std::thread(&ODDMA::stateProducer, this);
	std::thread stateConsumerThread = std::thread(&ODDMA::stateConsumer, this);

	stateProducerThread.detach();
	stateConsumerThread.detach();

	std::thread commandListenerThread = std::thread(&ODDMA::commandListener, this);
	commandListenerThread.detach();
}

void ODDMA::stop()
{
	threadsStarted = false;		
	while (threadsStopped != 2); //waiting for all threds to finish
	std::queue<RocketStatus> empty;
	std::swap(qStatuses, empty);
}


void ODDMA::restart()
{
	stop();
	start();
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
		status.timestamp = runningTime; //duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
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

		takeANap();
 	}

	threadsStopped++;
}

void ODDMA::stateConsumer()
{
	while (threadsStarted)
	{
		while (statusSemaphore);
		if (qStatuses.size() > 0)
		{
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
		}

		takeANap();
	}

	threadsStopped++;
}

void ODDMA::sendCommandChangeThrust(double thrustMagnitude)
{
	physics->updateThrustMagnitude(thrustMagnitude);
}

void ODDMA::sendCommandChangeDirectionX(double angle)
{
	if (physics->getThrustMagnitude() > 0.01)
	{
		glm::dvec3 delta = glm::dvec3(angle, 0, 0);
		physics->rotateRocket(delta);
	}
}

void ODDMA::sendCommandChangeDirectionY(double angle)
{
	if (physics->getThrustMagnitude() > 0.01)
	{
		glm::dvec3 delta = glm::dvec3(0, angle, 0);
		physics->rotateRocket(delta);
	}
}

void ODDMA::sendCommandChangeDirectionZ(double angle)
{
	if (physics->getThrustMagnitude() > 0.01)
	{
		glm::dvec3 delta = glm::dvec3(0, 0, angle);
		physics->rotateRocket(delta);
	}
}

void ODDMA::sendCommandChangeGyroRotationX(double angle)
{
	if (physics->getThrustMagnitude() <= 0.01)
	{
		glm::dvec3 delta = glm::dvec3(angle, 0, 0);
		physics->rotateRocket(delta);
	}
}

void ODDMA::sendCommandChangeGyroRotationY(double angle)
{
	if (physics->getThrustMagnitude() <= 0.01)
	{
		glm::dvec3 delta = glm::dvec3(0, angle, 0);
		physics->rotateRocket(delta);
	}
}

void ODDMA::sendCommandChangeGyroRotationZ(double angle)
{
	if (physics->getThrustMagnitude() <= 0.01)
	{
		glm::dvec3 delta = glm::dvec3(0, 0, angle);
		physics->rotateRocket(delta);
	}
}

void ODDMA::executeInstruction(int instrCode, double value)
{
	switch (instrCode)
	{
		case 1:
		{
			sendCommandChangeThrust(value);
			break;
		}
		case 2:
		{
			sendCommandChangeDirectionX(value);
			break;
		}
		case 3:
		{
			sendCommandChangeDirectionY(value);
			break;
		}
		case 4:
		{
			sendCommandChangeDirectionZ(value);
			break;
		}
		case 5:
		{
			sendCommandChangeGyroRotationX(value);
			break;
		}
		case 6:
		{
			sendCommandChangeGyroRotationY(value);
			break;
		}
		case 7:
		{
			sendCommandChangeGyroRotationZ(value);
			break;
		}
		default:
			break;
	}

	std::cout << "Instruction executed, code: " + std::to_string(instrCode)
		+ ", value: " + std::to_string(value) + "\n";
}

void ODDMA::commandListener()
{
	while (threadsStarted)
	{
		if (commandBus->anyCommands())
		{
			RocketCommand rocketCommand = commandBus->getCommad(runningTime);
			executeInstruction(rocketCommand.code(), rocketCommand.value());
		}

		takeANap();
	}
}

void ODDMA::takeANap()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(4));
}
