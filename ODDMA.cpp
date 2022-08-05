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

		takeANap();
 	}
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
			address -= 8;			
		}

		takeANap();
	}
}

void ODDMA::sendCommandChangeThrust(RocketChangeThrust command)
{
	physics->updateThrustMagnitude(command.thrust);
}

void ODDMA::sendCommandChangeDirection(RocketChangeDirection command)
{
	//TODO: switch on!!!
	//physics->updateKeyPressed(command.directionCode);	
}

void ODDMA::commandListener()
{
	while (threadsStarted)
	{
		Memory* memory = vm->getMemory();
		unsigned char commandRecieved = memory->fetchByte(commandAddress);
		int address = commandAddress;
		if (commandRecieved == 0)
		{
			// read code:
			address++;
			unsigned char commandCode = memory->fetchByte(address);
			switch (commandCode)
			{
			case 1: 
			{
				// change direction;
				address++;
				unsigned char dirCode = memory->fetchByte(address);
				switch (dirCode)
				{
				case 1:
				{
					RocketChangeDirection changeDirection(Direction::DIRECTION_UP, 265);
					sendCommandChangeDirection(changeDirection);
					memory->storeByte(commandAddress, 1); // command recieved
					break;
				}
				case 2:
				{
					RocketChangeDirection changeDirection(Direction::DIRECTION_DOWN, 264);
					sendCommandChangeDirection(changeDirection);
					memory->storeByte(commandAddress, 1); // command recieved
					break;
				}
				case 3:
				{
					RocketChangeDirection changeDirection(Direction::DIRECTION_LEFT, 263);
					sendCommandChangeDirection(changeDirection);
					memory->storeByte(commandAddress, 1); // command recieved
					break;
				}
				case 4:
				{
					RocketChangeDirection changeDirection(Direction::DIRECTION_RIGHT, 262);
					sendCommandChangeDirection(changeDirection);
					memory->storeByte(commandAddress, 1); // command recieved
					break;
				}
				default:
					break;
				}

				break;
			}
			case 2:
			{
				address++;
				double thrust = memory->fetchDWord(address);
				RocketChangeThrust changeThrust(thrust);
				sendCommandChangeThrust(changeThrust);
				memory->storeByte(commandAddress, 1); // command recieved
				break;
			}
			default:
				// 0 - do nothing
				break;
			}	
		}

		takeANap();
	}
}

void ODDMA::takeANap()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(4));
}
