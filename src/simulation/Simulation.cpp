#include "Simulation.h"

#include "../infrastructure/FileService.h"

using namespace ofsim_events;
using namespace ofsim_world;
using namespace ofsim_math_and_physics;
using namespace ofsim_simulation;

Simulation::Simulation()
{
	camera = std::make_unique<ofsim_renderer::Camera>(glm::vec3(-100.0, -160.0, 1000.0));
	mainWindow = std::make_unique<Window>(*camera, SCR_WIDTH, SCR_HEIGHT);
	initWindowContext();

	solarSystem = std::make_unique<SolarSystem>();
	
	// rocket:
    glm::dvec3 rocketPos = rocketInitialPosition(theta, phi);
    rocket = std::make_unique<Rocket>("model3d_shader", rocketPos, rocket_initial_size);
	
	// state machine:
	eventDispatcher = std::make_unique<EventDispatcher>();
	stateMachine = std::make_unique<StateMachine>(*eventDispatcher);
	smEventListener = std::make_unique<SMEventsListener>(*eventDispatcher);

    camera->position = rocket->properties().position + glm::dvec3(0.0, 0.024, 0.0);	
}

// ----- simulation initialization methods

void Simulation::init()	
{
	lag = 0;
	previous = currentTime();
	projection = glm::perspective((double)glm::radians(camera->Zoom),
		(double)SCR_WIDTH / (double)SCR_HEIGHT, 0.001, 150000000.0);		
}

void Simulation::physicsRocketInitialOrientation()
{    
    RocketPhysicalProperties& rocketProperties = rocket->properties();
    physics = std::make_unique<ofsim_math_and_physics::PhysicsSolver>(
		rocketProperties,
		CelestialBodyType::planet,
		SolarSystemConstants::earthSize,
		MS_PER_UPDATE);
    physics->establishInitialOrientation(rocketInitialPosition(theta, phi), theta, phi);
}

void Simulation::initialOrbitalInformation()
{	
	lastAltitude = 0;
	apogeum = 0;
	perygeum = 0;
	lastAltitudeDirection = 1;
	altitudeDirection = 1;
}

/// ---- simualtion restart methods:

void Simulation::start()
{		
	mainLoop();
}

void Simulation::terminatePythonMachine()
{
	if (pythonThread != nullptr)
	{
		pythonMachine->terminateProgram();				
		pythonThread->join();	
		pythonMachine.reset();
		pythonThread.reset();
		pythonMachine = nullptr;
		pythonThread = nullptr;
	}
}

void Simulation::terminateVMachine()
{
	if (vmThread != nullptr)
	{
		vmachine->stop();
		vmThread->join();
		vmThread.reset();
		vmachine.reset();		
		vmachine = nullptr;
		vmThread = nullptr;
	 }
}

void Simulation::stop()
{
	terminatePythonMachine();
	
	simulationMode = SimulationMode::WAITING_FOR_BEGIN;
	
	// dispose rocket and physics:
	rocket.reset();
	rocket = nullptr;
    glm::dvec3 rocketPos = rocketInitialPosition(theta, phi);

    rocket = std::make_unique<Rocket>("model3d_shader", rocketPos, rocket_initial_size);
	
	physics.reset();
	physics = nullptr;
	physicsRocketInitialOrientation();	
		
	EventProcessor::getInstance()->clearCommandHistory();

	initialSolarSystemInformation();
	initialOrbitalInformation();
	physics->reset();
			
	runningTime = 0;
	telemetryCollector->clear();

	EventProcessor::getInstance()->povideRocketAndPhysics(rocket.get(), physics.get());
	EventProcessor::getInstance()->terminatePythonMachine(false);
}

void Simulation::mainLoop()
{		
	// <---- initialization section; ----->

	// initialize physics solver:
    physicsRocketInitialOrientation();

    // initialize communication Bus and telemetry collector:	
	telemetryCollector = std::make_unique<TelemetryCollector>();

	trajectoryPrediction = std::make_unique<TrajectoryPrediction>();

	// initialize GUI:
	createGui();	

	// init time variables:
	startTime = currentTime();
	runningTime = 0;
	timePaused = 0;

	initialSolarSystemInformation();
	initialOrbitalInformation();

	skyboxRenderer = std::make_unique<SkyBoxRenderer>();
	skyboxRenderer->init();

	f64 radius{ 0.000000001 };
	f64 step{ 0.000000001 };
	
	EventProcessor::getInstance()->povideRocketAndPhysics(rocket.get(), physics.get());

	// <----- end of initialization section ----->

	dvec3 toTheMoon = SolarSystemConstants::moonPos;
	bool frwd = true;
	while (!mainWindow->shouldClose())
	{	
		EventProcessor::getInstance()->provideRunnigTume(runningTime);		
        int factor = gui->getTimeFactor();
				
        // calculate lag:
		if (factor == 0) // simulation paused
		{
			timePaused = currentTime() - previous;			
		}
        else if (factor > 0) // simulation running
		{
			u64 current = currentTime() - timePaused;
			u64 elapsed = (current - previous) * factor;
			previous = current;
			lag += elapsed;
			runningTime += elapsed;						
		}		

		// input processing:		
		userInteractionLogic(toTheMoon, radius, step);

		if (simulationMode == SimulationMode::STANDARD_SIMULATION) 
		{	
			lag = physics->calculateForces(lag);					
		}

        std::vector<float> rgb = physics->atmosphereRgb();
        switchGLStateForWorldRendering(rgb[0], rgb[1], rgb[2]);

		gui->newFrame();
		
		// camera/view transformation:
		if (simulationMode == SimulationMode::STANDARD_SIMULATION 
			|| simulationMode == SimulationMode::WAITING_FOR_BEGIN)
		{
			camera->setAutomaticRotation(true);
            camera->updatePosition(rocket->properties().position);
			camera->processCameraRotation(3.0, 0);
		}
		else 
        {
            if (simulationMode == SimulationMode::PRESENTATION_MODE)
			{
				camera->setAutomaticRotation(true);				
                toTheMoon = SolarSystemConstants::moonPos - rocket->properties().position;
				
                camera->updatePosition(rocket->properties().position + (toTheMoon * radius));
				
				if (radius < 0.52)
				{ 
					step *= 1.005;
				}
				else
				{
					step /= 1.005;
				}

				radius += step;							
			}
		}

        glm::dmat4 view = camera->getViewMatrix();

		// render world:
		solarSystem->render(projection, view, SolarSystemConstants::lightPos);
		rocket->render(projection, view, SolarSystemConstants::lightPos);

		if (simulationMode == SimulationMode::TRAJECTORY_PREDICTION || simulationMode == SimulationMode::PRESENTATION_MODE)
		{
			trajectoryPrediction->render(projection, view, SolarSystemConstants::lightPos);
			
			if (simulationMode == SimulationMode::PRESENTATION_MODE)
			{
                double distance = glm::length(camera->position - rocket->properties().position);
				gui->renderPresentationModeInfo(distance);
			}
		}

        if (physics->altitude > skybox_rendering_boundary)
        {
            skyboxRenderer->render(projection, view, camera.get());
        }

        if (simulationMode == SimulationMode::DIAGNOSTICS_MODE)
        {
            skyboxRenderer->render(projection, view, camera.get());
        }

		collectTelemetry();
		
		renderHUD();

		calcApogeumAndPerygeum();

		lastAltitudeDirection = altitudeDirection;
        lastAltitude = physics->altitude;
	
		// sync and swap:
		syncFramerate(currentTime(), MS_PER_UPDATE);
		mainWindow->swapBuffers();
		glfwPollEvents();
	}

	simulationMode = SimulationMode::FINISHED;

	terminatePythonMachine();	
	terminateVMachine();	
}

void Simulation::renderHUD()
{
	gui->renderMenuBar();
	gui->renderSplashScreen();
	gui->renderFileSaveAsDialog();
	gui->renderFileOpenDialog();	

    if (simulationMode == SimulationMode::DIAGNOSTICS_MODE)
    {
        auto diagnostics = prepareDiagnosticsData();
        gui->renderDiagnostics(diagnostics);
    }
	else
	{
		gui->renderSimulationControlWindow(runningTime);
		gui->renderCodeEditor(orbitalProgramSourceCode);	

		gui->renderCommandHistory(ofsim_events::EventProcessor::getInstance()->getCommandHistory());

		gui->plotTelemetry(
            telemetryCollector->velocityHistory, telemetryCollector->maxVelocity,
            telemetryCollector->altitudeHistory, telemetryCollector->maxAltitude,
            telemetryCollector->atmPressureHistory, telemetryCollector->maxAtmPressure,
            telemetryCollector->accelerationHistory, telemetryCollector->maxAcceleration, telemetryCollector->minAcceleration);
        renderTelemetry(gui.get(), rocket.get(), physics->altitude, apogeum, perygeum, physics->getAtmosphereDragForceMagnitude());

		gui->renderTranslationErrors(EventProcessor::getInstance()->getPythonError());

        if (showDiagnosticsInSimulation)
        {
            auto diagnostics = prepareDiagnosticsData();
            gui->renderDiagnostics(diagnostics);
        }
	}

	gui->endRendering();
}

DiagnosticsData Simulation::prepareDiagnosticsData()
{
    auto properties = rocket->properties();
    DiagnosticsData diagnostics;
    diagnostics.rocketPosition = properties.position;
    diagnostics.rocketRotation = properties.rotation;
    diagnostics.thrustVectorDirection = physics->thrustVector;
    diagnostics.cameraPosition = camera->position;
    diagnostics.dTheta = dTheta;
    diagnostics.dPhi = dPhi;

    return diagnostics;
}

/**
 * @brief User interaction with the simulation
 * @details Interprets user events recieved from the GUI
*/
void Simulation::userInteractionLogic(dvec3& toTheMoon, f64& radius, f64& step)
{	
	// recieve and interpret user events:
	SimulationEvent event = EventProcessor::getInstance()->getEvent();

	if (event.action == UserAction::PYTHON_PROGRAM_EXECUTION_STOP)
	{
		if (simulationMode == SimulationMode::STANDARD_SIMULATION)
		{				
			stop();						
		}
	}

	if (event.action == UserAction::PROGRAM_FILE_OPEN)
	{		
		// load program source code from file (Python or VM):
		orbitalProgramName = event.data;
		orbitalProgramSourceCode = ofsim_infrastructure::loadSourceCode(event.data);
	}

	if (event.action == UserAction::PROGRAM_TRANSLATE)
	{
		if (this->orbitalProgramSourceCode.empty())
		{
			std::cout << "No source code to execute!\n";
			return;
		}
		
		if (this->orbitalProgramName.find(".py") != std::string::npos)
		{
			pythonMachine = std::make_unique<ofsim_python_integration::PythonMachine>();
			pythonThread = std::make_unique<std::thread>(&ofsim_python_integration::PythonMachine::runPythonOrbitalProgram,
				pythonMachine.get(), this->orbitalProgramSourceCode);
		}
		else if (this->orbitalProgramName.find(".oasm") != std::string::npos)
		{
			vmachine = std::make_unique<ofsim_vm::VMachine>(*EventProcessor::getInstance());
			vmachine->translateSourceCode(this->orbitalProgramSourceCode);
			vmThread = std::make_unique<std::thread>(&ofsim_vm::VMachine::start, vmachine.get());
		} 
		else 
		{
			std::cout << "Unknown file extension!\n";
			return;
		}
				
		simulationMode = SimulationMode::STANDARD_SIMULATION;
	}

	if (event.action == UserAction::PYTHON_PROGRAM_RAISED_ERROR)
	{
		gui->setTimeFactor(-1);
		stop();
	}

	if (event.action == UserAction::FILE_SAVE)
	{
		ofsim_infrastructure::saveSourceCode(SOURCE_CODE_FILE_NAME, orbitalProgramSourceCode);		
	}

	if (event.action == UserAction::FILE_SAVED_AS)
	{
		std::string fileSaved = gui->getSavedFile();
		ofsim_infrastructure::saveSourceCode(fileSaved, orbitalProgramSourceCode);
	}

	if (event.action == UserAction::FILE_EXIT)
	{
		glfwSetWindowShouldClose(mainWindow->getWindow(), true);
	}

	if (event.action == UserAction::CHANGE_MODE_TO_FROM_DIAGNOSTICS) // o
	{
		if (simulationMode == SimulationMode::WAITING_FOR_BEGIN)
		{
            // entering diagnostics mode!
            std::cout << "Diagnostics_mode! \n" << std::flush;
            simulationMode = SimulationMode::DIAGNOSTICS_MODE;

            physics->establishInitialOrientation(rocketInitialPosition(theta, phi), theta, phi);
            RocketPhysicalProperties &rocketProperties = rocket->properties();
            rocketProperties.size *= 300000;
            camera->updatePosition(solarSystem->pointAboveEarthSurface(phi, theta - 20, 14421.0));
            camera->setAutomaticRotation(false);
		}
		else if (simulationMode == SimulationMode::DIAGNOSTICS_MODE)
		{
            std::cout << "Waiting for begin mode! \n" << std::flush;
            stop(); // reseting the rocket, physics and vm states;
			simulationMode = SimulationMode::WAITING_FOR_BEGIN;
            camera->setAutomaticRotation(true);
            dTheta = theta;
            dPhi = phi;
		}
	}

    if (simulationMode == SimulationMode::DIAGNOSTICS_MODE)
    {
        switch (event.action)
        {
            case (UserAction::ROTATION_LATITUDE_UP):
                std::cout << "rotation latitude UP \n" << std::flush;
                dTheta += 10;
            break;
            case (UserAction::ROTATION_LATITUDE_DOWN):
                std::cout << "rotation latitude DOWN \n" << std::flush;
                dTheta -= 10;
            break;
            case (UserAction::ROTATION_LONGITUDE_UP):
                std::cout << "rotation longitude UP \n" << std::flush;
                dPhi += 10;
            break;
            case (UserAction::ROTATION_LONGITUDE_DOWN):
                std::cout << "rotation longitude DOWN \n" << std::flush;
                dPhi -= 10;
            break;
            default: ; // do nthng!
        }

        bool diagAction = event.action == UserAction::ROTATION_LATITUDE_UP
                || event.action == UserAction::ROTATION_LATITUDE_DOWN
                || event.action == UserAction::ROTATION_LONGITUDE_UP
                || event.action == UserAction::ROTATION_LONGITUDE_DOWN;
        if (diagAction)
        {            
            dvec3 rocketPosition = rocketInitialPosition(dTheta, dPhi);
            physics->establishInitialOrientation(rocketPosition, dTheta, dPhi);
            rocket->properties().position = rocketPosition;
        }
    }

	if (event.action == UserAction::CHANGE_MODE_TO_FORM_PRESENTATION 
		 || event.action == UserAction::CHANGE_MODE_TO_FROM_PREDICTION) // m, k
	{		
		// change to presention or prediction mode is possible only when simulation is not waiting for begin:
		if (simulationMode != SimulationMode::WAITING_FOR_BEGIN)
		{			
			camera->setAutomaticRotation(false);
			physics->predictTrajectory(runningTime);
			trajectoryPrediction->initWithPositions(
                physics->trajectoryPredictionX,
                physics->trajectoryPredictionY,
                physics->trajectoryPredictionZ,
                telemetryCollector->telemetryHistory);

			if (event.action == UserAction::CHANGE_MODE_TO_FROM_PREDICTION) // m
            {
				if (simulationMode != SimulationMode::TRAJECTORY_PREDICTION)
				{
                    camera->updatePosition(solarSystem->pointAboveEarthSurface(35, 35, 7521.0));
					simulationMode = SimulationMode::TRAJECTORY_PREDICTION;
				}
				else
				{
					simulationMode = SimulationMode::STANDARD_SIMULATION;
				}
			}

			if (event.action == UserAction::CHANGE_MODE_TO_FORM_PRESENTATION) // k
			{
				if (simulationMode != SimulationMode::PRESENTATION_MODE)
				{
                    toTheMoon = rocket->properties().position - SolarSystemConstants::moonPos;
					radius = 0.000000001;
					step = 0.000000001;
					simulationMode = SimulationMode::PRESENTATION_MODE;
				}
				else
				{
                    simulationMode = SimulationMode::STANDARD_SIMULATION;
					gui->restoreWindows();
				}
			}
		}
	}

    if (event.action == UserAction::SHOW_DIAGNOSTICS_IN_SIMULATION)
    {
        showDiagnosticsInSimulation = !showDiagnosticsInSimulation;
    }
}

unsigned long long Simulation::currentTime()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
		).count();
}

void Simulation::createGui()
{
	gui = std::make_unique<ofsim_gui::Gui>();
	gui->initialization(mainWindow.get());
	gui->loadTextures();
}

void Simulation::collectTelemetry()
{
    u64 tickTock = runningTime / 1000;
	if (runningTime > 0)
	{
		TelemetryData data;
		data.time = tickTock;
        data.altitude = physics->altitude;
		data.apogeum = apogeum;
		data.perygeum = perygeum;
		data.atmPressure = physics->getAtmosphereDragForceMagnitude();
        data.mass = rocket->properties().mass;
        data.velocity = glm::length(rocket->properties().velocity);
        data.position = rocket->properties().position;

		telemetryCollector->registerTelemetry(data);
	}
}

void Simulation::initWindowContext()
{
	int result = mainWindow->initialize();
	if (result != 0)
	{
		std::cout << "Main window creation failed. Status code = " << result << "\n";
    }
}

void Simulation::switchGLStateForWorldRendering(float r, float g, float b)
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(r, g, b, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Simulation::calcApogeumAndPerygeum()
{
    if (lastAltitude < physics->altitude && lastAltitude > 4)
	{
		altitudeDirection = 1;
	}

    if (lastAltitude > physics->altitude && lastAltitude > 4)
	{
		altitudeDirection = -1;
	}

	if (lastAltitudeDirection == 1 && altitudeDirection == -1)
	{
		apogeum = lastAltitude;
	}

	if (lastAltitudeDirection == -1 && altitudeDirection == 1)
	{
		perygeum = lastAltitude;
	}
}

void Simulation::renderTelemetry(ofsim_gui::Gui* gui, Rocket* rocket, 
	double altitude, double apogeum, double perygeum, double atmosphereDragForceMagnitude)
{
	TelemetryData data;
	
	data.altitude = altitude;
    data.mass = rocket->properties().mass;
	data.atmPressure = atmosphereDragForceMagnitude;
    glm::dvec3 velocity = rocket->properties().velocity;
	data.velocity = glm::length(velocity);

    data.position = rocket->properties().position;

	data.apogeum = apogeum;
	data.perygeum = perygeum;

	gui->renderTelemetry(data);
}

void Simulation::syncFramerate(u64 startTime, i32 ms_per_update)
{
	u64 endTime = startTime + ms_per_update;
	while (currentTime() < endTime)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}
