#include "Simulation.h"

using namespace ofsim_events;

Simulation::Simulation()
{
	camera = std::make_unique<ofsim_renderer::Camera>(glm::vec3(-100.0, -160.0, 1000.0));
	mainWindow = std::make_unique<Window>(*camera, SCR_WIDTH, SCR_HEIGHT);
	initWindowContext();

	solarSystem = std::make_unique<SolarSystem>();
	
	// rocket:
	glm::dvec3 rocketPos = solarSystem->pointAboveEarthSurface(angle, dangle, -0.2);
	rocket = std::make_unique<Rocket>("model3d_shader", rocketPos, 0.000013);
	
	camera->position = rocket->getPosition() + glm::dvec3(0.0, 0.024, 0.0);	
}

void Simulation::init()	
{
	lag = 0;
	previous = currentTime();
	projection = glm::perspective((double)glm::radians(camera->Zoom),
		(double)SCR_WIDTH / (double)SCR_HEIGHT, 0.001, 150000000.0);		
}

void Simulation::start()
{
	simulationStopped = 0;
	mainLoop();
}

void Simulation::stop()
{
	simulationStopped = true;
}

void Simulation::mainLoop()
{	
	// <---- initialization sectiom; ----->

	// initialize physics solver:
	glm::dvec3 towards = solarSystem->pointAboveEarthSurface(angle, dangle, -50.0);
	RocketPhysicalProperties rocketProperties = rocket->projectProperties();
	physics = std::make_unique<ofsim_math_and_physics::PhysicsSolver>(rocketProperties, MS_PER_UPDATE);
	physics->changeInitialAltitudeOrientation(CelestialBodyType::planet, 3185.0, towards);

	// initialize communication Bus and telemetry collector:	
	telemetryCollector = std::make_unique<TelemetryCollector>();

	trajectoryPrediction = std::make_unique<TrajectoryPrediction>();

	// initialize GUI:
	createGui();	

	// init time variables:
	startTime = currentTime();
	runningTime = 0;
	timePaused = 0;

	initialPhysicsInformation();
	initialOrbitalInformation();

	skyboxRenderer = std::make_unique<SkyBoxRenderer>();
	skyboxRenderer->init();

	f64 radius{ 0.000000001 };
	f64 step{ 0.000000001 };

	// <------ initialize Virtual Machine and ODDMA: ------>
	this->communicationBus = std::make_unique<com_bus::Tbus_data>();
	this->vm = std::make_unique<ofsim_vm::VMachine>(communicationBus.get());
	this->oddma = std::make_unique<ODDMA>(*rocket, *physics, *vm, *communicationBus);	

	// <----- end of initialization section ----->

	dvec3 toTheMoon = SolarSystemConstants::moonPos;
	bool frwd = true;
	while (!mainWindow->shouldClose())
	{		
		int factor = gui->getTimeFactor();
				
		oddma->provideRunningTime(runningTime);

		// calculate lag:       
		if (factor == 0)
		{
			timePaused = currentTime() - previous;			
		}
		else if (factor > 0)
		{
			unsigned long long current = currentTime() - timePaused;
			unsigned long long elapsed = (current - previous) * factor;
			previous = current;
			lag += elapsed;
			runningTime += elapsed;
			simulationStopped = false;			
		}		

		// input
		mainWindow->processInput();
		userInteraction(toTheMoon, radius, step);

		if (simulationStopped != 1) 
		{	
			lag = physics->calculateForces(lag);					
		}
		
		std::vector<float> rgb = physics->atmosphereRgb();
		switchGLStateForWorldRendering(rgb[0], rgb[1], rgb[2]);

		gui->newFrame();
		
		// camera/view transformation:
		if (!trajectoryPredictionMode && !presentationMode)
		{
			camera->setAutomaticRotation(true);
			camera->updatePosition(rocket->getPosition(), rocket->getRotation());
			camera->processCameraRotation(3.0, 0);
		}
		else 
		{			
			if (presentationMode)
			{
				camera->setAutomaticRotation(true);				
				toTheMoon = SolarSystemConstants::moonPos - rocket->getPosition();
				
				camera->updatePosition(rocket->getPosition() + (toTheMoon * radius), rocket->getRotation());
				
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

		if (trajectoryPredictionMode || presentationMode)
		{
			trajectoryPrediction->render(projection, view, SolarSystemConstants::lightPos);
			
			if (presentationMode)
			{
				double distance = glm::length(camera->position - rocket->getPosition());
				gui->renderPresentationModeInfo(distance);
			}
		}

		if (physics->getAltitude() > 27.0)
		{
			skyboxRenderer->render(projection, view, camera.get());
		}

		collectTelemetry();
		
		renderHUD();

		calcApogeumAndPerygeum();

		lastAltitudeDirection = altitudeDirection;
		lastAltitude = physics->getAltitude();
	
		// sync and swap:
		syncFramerate(currentTime(), MS_PER_UPDATE);
		mainWindow->swapBuffers();
		glfwPollEvents();
	}

	if (vm->isStarted()) 
	{
		vm->stop();
		vmThread->join();
	}
}

void Simulation::renderHUD()
{
	gui->renderMenuBar();
	gui->renderSplashScreen();
	gui->renderFileSaveAsDialog();
	gui->renderFileOpenDialog();
	gui->renderSimulationControlWindow(runningTime);
	gui->renderCodeEditor(orbitalProgramSourceCode);
	std::map<unsigned long long, RocketCommand>& commandHistory = communicationBus->command_history;
	gui->renderCommandHistory(commandHistory);

	gui->plotTelemetry(
		telemetryCollector->getVelicityHistory(), telemetryCollector->getMaxVelocity(),
		telemetryCollector->getAltitudeHistory(), telemetryCollector->getMaxAltitude(),
		telemetryCollector->getAtmPressureHistory(), telemetryCollector->getMaxAtmPressure(),
		telemetryCollector->getAccelarationHistory(), telemetryCollector->getMaxAcceleration(), telemetryCollector->getMinAcceleration());
	renderTelemetry(gui.get(), rocket.get(), physics->getAltitude(), apogeum, perygeum, physics->getAtmosphereDragForceMagnitude());

	gui->endRendering();
}

/**
 * @brief User interaction with the simulation
 * @details Interprets user events recieved from the GUI
*/
void Simulation::userInteraction(dvec3& toTheMoon, f64& radius, f64& step)
{	
	// recieve and interpret user events:
	UserEvent event = EventProcessor::getInstance()->getUserEvent();

	if (event.action == UserAction::PROGRAM_FILE_OPENED)
	{
		std::cout << "Event received: " << (int)event.action << "\n";
		std::cout << "Data reciewed " << event.data << "\n";

		// load VM program source code from file:
		this->orbitalProgramSourceCode = vm->translateSourceCodeFromFile(event.data.c_str());
	}

	if (event.action == UserAction::PROGRAM_START_EXECUTION)
	{
		// TODO: consider hide thread execution (simillar to ODDMA):
		vmThread = std::make_unique<std::thread>(&ofsim_vm::VMachine::start, this->vm.get());
		oddma->start();
	}

	if (event.action == UserAction::FILE_SAVE)
	{
		ofsim_infrastructure::FileService::saveSourceCode(SOURCE_CODE_FILE_NAME, orbitalProgramSourceCode);		
	}

	if (event.action == UserAction::FILE_SAVED_AS)
	{
		std::string fileSaved = gui->getSavedFile();
		ofsim_infrastructure::FileService::saveSourceCode(fileSaved, orbitalProgramSourceCode);
	}

	if (event.action == UserAction::FILE_EXIT)
	{
		glfwSetWindowShouldClose(mainWindow->getWindow(), true);
	}

	if (event.action == UserAction::CHANGE_MODE_TO_FORM_PRESENTATION 
		 || event.action == UserAction::CHANGE_MODE_TO_FROM_PREDICTION) // m, k
	{
	 	camera->setAutomaticRotation(false);
	 	physics->predictTrajectory(runningTime);
	 	trajectoryPrediction->initWithPositions(
	 		physics->getTrajectoryPredictionX(),
	 		physics->getTrajectoryPredictionY(),
	 		physics->getTrajectoryPredictionZ(),
	 		telemetryCollector->getTelemetryHistory());

	 	if (event.action == UserAction::CHANGE_MODE_TO_FORM_PRESENTATION) // m
	 	{
	 		if (trajectoryPredictionMode == false)
	 		{
	 			camera->updatePosition(solarSystem->pointAboveEarthSurface(30, 30, 800), rocket->getRotation());
	 			trajectoryPredictionMode = true;
	 		}
	 		else
	 		{
	 			trajectoryPredictionMode = false;
	 		}
	 	}

	 	if (event.action == UserAction::CHANGE_MODE_TO_FROM_PREDICTION) // k
	 	{
	 		if (presentationMode == false)
	 		{
	 			toTheMoon = rocket->getPosition() - SolarSystemConstants::moonPos;
	 			radius = 0.000000001;
	 			step = 0.000000001;
	 			presentationMode = true;
	 		}
	 		else
	 		{
	 			presentationMode = false;
	 			gui->restoreWindows();
	 		}
	 	}
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
	unsigned long long tickTock = runningTime / 1000;
	if (runningTime > 0)
	{
		TelemetryData data;
		data.time = tickTock;
		data.altitude = physics->getAltitude();
		data.apogeum = apogeum;
		data.perygeum = perygeum;
		data.atmPressure = physics->getAtmosphereDragForceMagnitude();
		data.mass = rocket->getMass();
		data.velocity = glm::length(rocket->getVelocity());		
		data.position = rocket->getPosition();

		telemetryCollector->registerTelemetry(data);
	}
}

void Simulation::initialPhysicsInformation()
{
	initialRocketRotation();
	solarSystem->provideRocketInformationAndInit(angle, dangle, rocket.get());	
}

void Simulation::initialRocketRotation()
{
	glm::dvec3 newRotation = glm::dvec3(-50.000021, 48.8000050, 0.0);
	glm::dvec3 deltaRotation = newRotation - rocket->getRotation();

	physics->rotateRocket(deltaRotation);
}

void Simulation::initialOrbitalInformation()
{
	simulationStopped = 1;
	lastAltitude = 0;
	apogeum = 0;
	perygeum = 0;
	lastAltitudeDirection = 1;
	altitudeDirection = 1;
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
	if (lastAltitude < physics->getAltitude() && lastAltitude > 4)
	{
		altitudeDirection = 1;
	}

	if (lastAltitude > physics->getAltitude() && lastAltitude > 4)
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

void Simulation::renderTelemetry(ofsim_gui::Gui* gui, Rocket* rocket, double altitude, double apogeum, double perygeum, double atmosphereDragForceMagnitude)
{
	TelemetryData data;
	
	data.altitude = altitude;
	data.mass = rocket->getMass();
	data.atmPressure = atmosphereDragForceMagnitude;
	glm::dvec3 velocity = rocket->getVelocity();
	data.velocity = glm::length(velocity);

	data.position = rocket->getPosition();

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