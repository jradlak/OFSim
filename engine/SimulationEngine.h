#pragma once

#include <vector>
#include <iostream>

#include "../renderer/Renderable.h"
#include "Task.h"
#include "../renderer/Camera.h"
#include "../renderer/Window.h"
#include "../gui/Gui.h"
#include "../world/solar_system/SolarSystem.h"
#include "../math_and_physics/PhysicsEngine.h"
#include "../vmachine/CommunicationBus.h"
#include "../vmachine/VMachine.h"
#include "../vmachine/ODDMA.h"

class SimulationEngine : public Task
{
public:
	SimulationEngine();

	void init();

	void start();

	void stop();

	void restart();

	~SimulationEngine();

private:
	const int MS_PER_UPDATE = 12;

	const unsigned int SCR_WIDTH = 1600;
	const unsigned int SCR_HEIGHT = 900;

	std::vector<Renderable*> renderables;		
	std::vector<Task*> backgroundTasks;

	// render matrices:
	glm::dmat4 projection;	

	SolarSystem* solarSystem;
	Rocket* rocket;
	PhysicsEngine* physics;
	
	// rocket and camera orientation:
	float angle = 30.0;
	float dangle = 60.0;
	
	// frame times:
	unsigned __int64 lag;
	unsigned __int64 previous;

	Camera* camera;
	Window* mainWindow;
	Gui* gui;

	// task and communication:
	CommunicationBus* communicationBus;
	VMachine* vm;
	ODDMA* oddma;
	
	// simulation time variables:
	unsigned __int64 startTime;
	unsigned __int64 runningTime;
	unsigned __int64 timePaused;
	
	// orbital orientation variables:
	int simulationStopped = 0;
	double lastAltitude = 0;
	double apogeum = 0;
	double perygeum = 0;
	int lastAltitudeDirection = 1;
	int altitudeDirection = 1;

	void initialPhysicsInformation();
	void initialOrbitalInformation();

	void initWindowContext();
	void mainLoop();

	void createGui();
	void loadSourceCode(std::string sourcePath);

	unsigned __int64 currentTime();
};