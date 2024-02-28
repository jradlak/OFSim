#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <memory>

#include "../world/SolarSystemConstants.h"
#include "TelemetryCollector.h"
#include "../infrastructure/FileService.h"
#include "../renderer/Camera.h"
#include "../renderer/Window.h"
#include "../renderer/TrajectoryPrediction.h"
#include "../renderer/TrajectoryPrediction.h"
#include "../renderer/SkyBoxRenderer.h"
#include "../gui/Gui.h"
#include "../world/SolarSystem.h"
#include "../math_and_physics/PhysicsSolver.h"
#include "../math_and_physics/MathTypes.h"
#include "../infrastructure/CommunicationBus.h"
#include "../infrastructure/ODDMA.h"
#include "../vmachine/VMachine.h"

enum class SimulationMode
{
	WAITING_FOR_BEGIN,
	STANDARD_SIMULATION,
	TRAJECTORY_PREDICTION,
	PRESENTATION_MODE
};

class Simulation
{
public:
	Simulation();

	void init();

	void start();

	void stop();

	~Simulation() {};

private:
	const i32 MS_PER_UPDATE{ 12 };
	
	const u32 SCR_WIDTH{ 1800 };
	const u32 SCR_HEIGHT{ 950 };

	//std::string SOURCE_CODE_FILE_NAME = "orbital_programs/ballisticProgram.oasm";
	std::string SOURCE_CODE_FILE_NAME{ "" };
	
	// render matrices:
	dmat4 projection;	

	// world objects:
	std::unique_ptr<SolarSystem> solarSystem;
	std::unique_ptr<Rocket> rocket;	
	
	// skybox:
	std::unique_ptr<SkyBoxRenderer> skyboxRenderer;

	// physics and trajectory prediction:
	std::unique_ptr<ofsim_math_and_physics::PhysicsSolver> physics;
	std::unique_ptr<TrajectoryPrediction> trajectoryPrediction;
		
	// camera, window and gui (user interaction objects):
	std::unique_ptr<ofsim_renderer::Camera> camera;
	std::unique_ptr<Window> mainWindow;
	std::unique_ptr<ofsim_gui::Gui> gui;

	// task and communication:
	std::unique_ptr<com_bus::Tbus_data> communicationBus;
	std::unique_ptr<ofsim_vm::VMachine> vm;
	std::unique_ptr<std::thread> vmThread;
	std::unique_ptr<ODDMA> oddma;
	std::unique_ptr<TelemetryCollector> telemetryCollector;

	// initial rocket and camera angles:
	f32 angle{ 30.0 };
	f32 dangle{ 60.0 };
	
	// frame times:
	u64 lag;
	u64 previous;
	
	// simulation time variables:
	u64 startTime;
	u64 runningTime;
	u64 timePaused;
	
	// orbital orientation variables:	
	f64 lastAltitude{ 0 };
	f64 apogeum{ 0 };
	f64 perygeum{ 0 };
	i32 lastAltitudeDirection{ 1 };
	i32 altitudeDirection{ 1 };

	SimulationMode simulationMode{ SimulationMode::WAITING_FOR_BEGIN };

	std::string orbitalProgramSourceCode { "" };

	RocketPhysicalProperties physicsRocketInitialOrientation();
	void initialPhysicsInformation();
	void initialRocketRotation();
	void initialOrbitalInformation();

	void initWindowContext();
    void mainLoop();

    void renderHUD();

    void userInteraction(dvec3 &toTheMoon, f64 &radius, f64 &step);

    void switchGLStateForWorldRendering(float r, float g, float b);
	void renderTelemetry(ofsim_gui::Gui* gui, Rocket* rocket, double altitude, double apogeum, double perygeum, double atmosphereDragForceMagnitude);
	void calcApogeumAndPerygeum();

	void syncFramerate(u64 startTime, i32 ms_per_update);

	void createGui();
		
	void collectTelemetry();

	u64 currentTime();	
};