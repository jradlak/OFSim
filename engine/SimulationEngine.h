#pragma once

#include <vector>
#include <iostream>

#include "../renderer/Renderable.h"
#include "Task.h"
#include "../renderer/Camera.h"
#include "../renderer/Window.h"
#include "../world/solar_system/SolarSystem.h"
#include "../math_and_physics/PhysicsEngine.h"
#include "../vmachine/CommunicationBus.h"

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

	Rocket* rocket;
	PhysicsEngine* physics;
	
	// rocket and camera orientation:
	float angle = 30.0;
	float dangle = 60.0;
	
	Camera* camera;
	Window* mainWindow;
	
	// task and communication:
	CommunicationBus* communicationBus;
	
	void initWindowContext();
	void mainLoop();
};