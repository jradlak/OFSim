#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <queue>
#include <chrono>
#include <thread>
#include <iostream>

#include "Memory.h"
#include "VMachine.h"
#include "../world/Rocket.h"
#include "../math_and_physics/PhysicsEngine.h"

struct RocketStatus
{
	double timestamp;
	glm::dvec3 velocity;
	glm::dvec3 position;
	glm::dvec3 rotation;
	double altitude;
	double thrustMagnitude;
	double mass;
};

enum CommandType {
	CHANGE_DIRECTION, CHANGE_THRUST
};

enum Direction {
	DIRECTION_LEFT, DIRECTION_RIGHT, DIRECTION_UP, DIRECTION_DOWN
};

//code = 1
class RocketChangeDirection
{
public:
	RocketChangeDirection(Direction newDirection, int newDirectionCode) 
	{ 
		direction = newDirection; 
		directionCode = newDirectionCode;
	}

	Direction direction;
	int directionCode;
};

//code = 2
class RocketChangeThrust
{
public:
	RocketChangeThrust(double newThrust) 
	{
		thrust = newThrust; 
	}

	double thrust;
};

// Orbital Data Direct Memory Access controller - ODDMA controller
class ODDMA : public Task
{
public:
	ODDMA(Rocket* _rocket, PhysicsEngine* _physics, VMachine* _vm, CommunicationBus* _commandBus);
	
	void start();
	
	void stop();

	void init();

	void restart();

	void executeInstruction(int instrCode, double value);

	void provideRunningTime(unsigned long long _runningTime) { runningTime = _runningTime; }
	
	~ODDMA() {}
private:
	// address in memory to store commands
	const unsigned int commandAddress = 65416;

	std::queue<RocketStatus> qStatuses;
	
	long lastCommandTimestamp;
	
	unsigned long long runningTime;

	Rocket* rocket;
	PhysicsEngine* physics;
	VMachine* vm;
	CommunicationBus* commandBus;

	bool threadsStarted;
	bool statusSemaphore;
	int threadsStopped = 0;

	void sendCommandChangeThrust(double thrustMagnitude);
	void sendCommandChangeDirectionX(double angle);
	void sendCommandChangeDirectionY(double angle);
	void sendCommandChangeDirectionZ(double angle);

	void sendCommandChangeGyroRotationX(double angle);
	void sendCommandChangeGyroRotationY(double angle);
	void sendCommandChangeGyroRotationZ(double angle);

	void stateProducer();
	void stateConsumer();
	void commandListener();

	void takeANap();
};