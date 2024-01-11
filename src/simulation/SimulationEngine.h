#pragma once

#include <vector>
#include <iostream>
#include <string>

#include "../world/SolarSystemConstants.h"
#include "../renderer/Renderable.h"
#include "Task.h"
#include "TelemetryCollector.h"
#include "../utils/FileService.h"
#include "../renderer/Camera.h"
#include "../renderer/Window.h"
#include "../renderer/TrajectoryPrediction.h"
#include "../renderer/TrajectoryPrediction.h"
#include "../renderer/SkyBoxRenderer.h"
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
	
	const unsigned int SCR_WIDTH = 1800;
	const unsigned int SCR_HEIGHT = 950;

	//std::string SOURCE_CODE_FILE_NAME = "orbital_programs/ballisticProgram.oasm";
	std::string SOURCE_CODE_FILE_NAME = "";

	std::vector<Renderable*> renderables;		
	std::vector<Task*> backgroundTasks;

	// render matrices:
	glm::dmat4 projection;	

	SolarSystem* solarSystem;
	Rocket* rocket;
	PhysicsEngine* physics;
	TrajectoryPrediction* trajectoryPrediction;
	SkyBoxRenderer* skyboxRenderer;

	// rocket and camera orientation:
	float angle = 30.0;
	float dangle = 60.0;
	
	// frame times:
	unsigned long long lag;
	unsigned long long previous;

	Camera* camera;
	Window* mainWindow;
	Gui* gui;
	
	// task and communication:
	CommunicationBus* communicationBus;
	VMachine* vm;
	ODDMA* oddma;
	TelemetryCollector* telemetryCollector;

	// simulation time variables:
	unsigned long long startTime;
	unsigned long long runningTime;
	unsigned long long timePaused;
	
	// orbital orientation variables:
	int simulationStopped = 0;
	double lastAltitude = 0;
	double apogeum = 0;
	double perygeum = 0;
	int lastAltitudeDirection = 1;
	int altitudeDirection = 1;

	bool trajectoryPredictionMode = false;	
	bool presentationMode = false;

	std::string orbitalProgramSourceCode;

	void initialPhysicsInformation();
	void initialRocketRotation();
	void initialOrbitalInformation();

	void initWindowContext();
	void mainLoop();

	void switchGLStateForWorldRendering(float r, float g, float b);
	void renderTelemetry(Gui* gui, Rocket* rocket, double altitude, double apogeum, double perygeum, double atmosphereDragForceMagnitude);
	void calcApogeumAndPerygeum();

	void syncFramerate(unsigned long long startTime, int ms_per_update);

	void createGui();
		
	void collectTelemetry();

	unsigned long long currentTime();
};