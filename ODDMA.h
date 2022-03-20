#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <queue>
#include <chrono>

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

class RocketCommand
{
public:
	double timestamp;
};

class RocketChangeRotation : RocketCommand 
{
public:
	glm::dvec3 newRotation;
};

class RocketChangeThrust : RocketCommand
{
public:
	double newThrustMagnitude;
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

	Rocket* rocket;
	PhysicsEngine* physics;
	VMachine* vm;

	bool threadsStarted;
	bool statusSemaphore;

	void publishState(RocketStatus state);
	RocketStatus getLastState();

	void sendCommand();

	void stateProducer();
	void stateConsumer();
	void commandListener();
};