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
#include "Rocket.h"
#include "PhysicsEngine.h"

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
	LEFT, RIGHT, UP, DOWN
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

class ODDMA
{
public:
	ODDMA(Rocket* _rocket, PhysicsEngine* _physics, VMachine* _vm);
	
	void start();
	void stop();
	
	~ODDMA() {}
private:
	std::queue<RocketStatus> qStatuses;
	
	long lastCommandTimestamp;
	int commandAddress;

	Rocket* rocket;
	PhysicsEngine* physics;
	VMachine* vm;

	bool threadsStarted;
	bool statusSemaphore;

	void sendCommandChangeThrust(RocketChangeThrust command);
	void sendCommandChangeDirection(RocketChangeDirection command);

	void stateProducer();
	void stateConsumer();
	void commandListener();

	void takeANap();
};