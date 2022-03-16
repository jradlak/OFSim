#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "VMachine.h"
#include "Rocket.h"

struct RocketState
{
	long timestamp;
	glm::dvec3 velocity;
	glm::dvec3 position;
	glm::dvec3 rotation;
	double altitude;
	double thrustMagnitude;
};

class RocketCommand
{
public:
	long timestamp;
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
	ODDMA(Rocket* _rocket, VMachine* _vm);
	
	void start();
	void stop();
	
	~ODDMA() {}
private:
	std::vector<RocketState> qStates;
	//std::vector<RocketCommand> qCommands;
	long lastCommandTimestamp;

	Rocket* rocket;
	VMachine* vm;

	void publishState(RocketState state);
	RocketState getLastState();

	void sendCommand();

	void stateProducer();
	void stateConsumer();
	void commandListiner();

};