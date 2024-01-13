#include "SimulationEngine.h"

int lastKeyPressed = 0;
void keyPressedCallback(int keyPressed);

SimulationEngine::SimulationEngine()
{
	camera = new Camera(glm::vec3(-100.0, -160.0, 1000.0));
	mainWindow = new Window(*camera, SCR_WIDTH, SCR_HEIGHT);
	initWindowContext();

	mainWindow->registerInputCallback(keyPressedCallback);

	solarSystem = new SolarSystem();
	
	// rocket:
	glm::dvec3 rocketPos = solarSystem->pointAboveEarthSurface(angle, dangle, -0.2);
	rocket = new Rocket("model3d_shader", rocketPos, 0.000013);
	
	camera->position = rocket->getPosition() + glm::dvec3(0.0, 0.024, 0.0);
	
	// physics:
	glm::dvec3 towards = solarSystem->pointAboveEarthSurface(angle, dangle, -50.0);
	physics = new PhysicsSolver(*rocket, MS_PER_UPDATE);
	physics->changeInitialAltitudeOrientation(CelestialBodyType::planet, 3185.0, towards);

	// initialize communication Bus and telemetry collector:
	communicationBus = new CommunicationBus();
	telemetryCollector = new TelemetryCollector();

	// initialize and start Virtual Machine:
	vm = new VMachine(communicationBus);
	//vm->translateSourceCode(SOURCE_CODE_FILE_NAME.c_str());
	//vm->start();

	// initialize and start ODDMA:
	oddma = new ODDMA(rocket, physics, vm, communicationBus);
	oddma->start();

	trajectoryPrediction = new TrajectoryPrediction();

	// initialize GUI:
	createGui();
	//orbitalProgramSourceCode =  FileService::loadSourceFile(SOURCE_CODE_FILE_NAME);

	// init time variables:
	startTime = currentTime();
	runningTime = 0;
	timePaused = 0;

	initialPhysicsInformation();
	initialOrbitalInformation();

	skyboxRenderer = new SkyBoxRenderer();
	skyboxRenderer->init();

	// register renderables:
	renderables.push_back(solarSystem);
	renderables.push_back(rocket);
}

void SimulationEngine::init()
{
	lag = 0;
	previous = currentTime();
	projection = glm::perspective((double)glm::radians(camera->Zoom),
		(double)SCR_WIDTH / (double)SCR_HEIGHT, 0.001, 150000000.0);
}

void SimulationEngine::start()
{
	simulationStopped = false;
	mainLoop();
}

void SimulationEngine::stop()
{
	simulationStopped = true;
}

void SimulationEngine::restart()
{	
	vm->unPause();
	runningTime = 0;
	//oddma->stop();
	vm->stop();
	communicationBus->clear();
	physics->reset();

	if (gui->getSelectedFile() != "")
	{
		SOURCE_CODE_FILE_NAME = gui->getSelectedFile();
		gui->clearSelectedFile();
	}

	orbitalProgramSourceCode = FileService::loadSourceFile(SOURCE_CODE_FILE_NAME);

	initialRocketRotation();
	initialOrbitalInformation();
	vm->provideSourcePath(SOURCE_CODE_FILE_NAME.c_str());
	vm->start();
	//oddma->start();
}

void SimulationEngine::mainLoop()
{	
	double radius = 0.000000001;
	double step = 0.000000001;

	glm::dvec3 toTheMoon = SolarSystemConstants::moonPos;
	bool frwd = true;
	while (!mainWindow->shouldClose())
	{		
		int factor = gui->getTimeFactor();
		
		// calculate lag:       
		if (factor == 0)
		{
			timePaused = currentTime() - previous;
			vm->setPause();
		}
		else if (factor > 0)
		{
			unsigned long long current = currentTime() - timePaused;
			unsigned long long elapsed = (current - previous) * factor;
			previous = current;
			lag += elapsed;
			runningTime += elapsed;
			vm->unPause();
			oddma->provideRunningTime(runningTime);
		}
		else if (factor == -1)
		{			
			restart();			
		}

		// input
		mainWindow->processInput();
		
		if (simulationStopped != 1) 
		{	
			// update physics:
			physics->updateRocketOrientation(lastKeyPressed);
			lag = physics->calculateForces(lag);
			
			if (gui->getLastClickedMenu() == ofsim_gui::MenuPosition::FILE_SAVE)
			{
				FileService::saveSourceCode(SOURCE_CODE_FILE_NAME, orbitalProgramSourceCode);
				gui->clearLastClickedMenu();
			}

			if (gui->getLastClickedMenu() == ofsim_gui::MenuPosition::FILE_SAVED_AS)
			{
				std::string fileSaved = gui->getSavedFile();
				FileService::saveSourceCode(fileSaved, orbitalProgramSourceCode);
				gui->clearLastClickedMenu();
			}
		
			if (gui->getLastClickedMenu() == ofsim_gui::MenuPosition::FILE_EXIT)
			{
				glfwSetWindowShouldClose(mainWindow->getWindow(), true);
				gui->clearLastClickedMenu();
			}

			if (lastKeyPressed == 77 || lastKeyPressed == 75) // m, k
			{
				camera->setAutomaticRotation(false);
				physics->predictTrajectory(runningTime);
				trajectoryPrediction->initWithPositions(
					physics->getTrajectoryPredictionX(),
					physics->getTrajectoryPredictionY(),
					physics->getTrajectoryPredictionZ(),
					telemetryCollector->getTelemetryHistory());

				if (lastKeyPressed == 77) // m
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

				if (lastKeyPressed == 75) // k
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

			lastKeyPressed = 0;
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
				//camera->processCameraRotation(3.0, 0);
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

		// render renderables:
		for (int i = 0; i < renderables.size(); i++)
		{
			renderables[i]->render(projection, view, SolarSystemConstants::lightPos);
		}

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
			skyboxRenderer->render(projection, view, camera);
		}

		// render HUD:
		// TODO: extract to separate method!
		gui->renderMenuBar();
		gui->renderSplashScreen();
		gui->renderFileSaveAsDialog();
		gui->renderFileOpenDialog();
		gui->renderSimulationControlWindow(runningTime);
		gui->renderCodeEditor(orbitalProgramSourceCode);
		std::map<unsigned long long, RocketCommand>& commandHistory = communicationBus->getCommandHistory();
		gui->renderCommandHistory(commandHistory);		
		
		collectTelemetry();
		gui->plotTelemetry(
			telemetryCollector->getVelicityHistory(), telemetryCollector->getMaxVelocity(),
			telemetryCollector->getAltitudeHistory(), telemetryCollector->getMaxAltitude(),
			telemetryCollector->getAtmPressureHistory(), telemetryCollector->getMaxAtmPressure(),
			telemetryCollector->getAccelarationHistory(), telemetryCollector->getMaxAcceleration(), telemetryCollector->getMinAcceleration());
		renderTelemetry(gui, rocket, physics->getAltitude(), apogeum, perygeum, physics->getAtmosphereDragForceMagnitude());
		
		gui->endRendering();

		calcApogeumAndPerygeum();

		lastAltitudeDirection = altitudeDirection;
		lastAltitude = physics->getAltitude();
	
		// sync and swap:
		syncFramerate(currentTime(), MS_PER_UPDATE);
		mainWindow->swapBuffers();
		glfwPollEvents();
	}
}

unsigned long long SimulationEngine::currentTime()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
		).count();
}

void SimulationEngine::createGui()
{
	gui = new ofsim_gui::Gui();
	gui->initialization(mainWindow);
	gui->loadTextures();
}

void SimulationEngine::collectTelemetry()
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

void SimulationEngine::initialPhysicsInformation()
{
	initialRocketRotation();
	solarSystem->provideRocketInformationAndInit(angle, dangle, rocket);	
}

void SimulationEngine::initialRocketRotation()
{
	glm::dvec3 newRotation = glm::dvec3(-50.000021, 48.8000050, 0.0);
	glm::dvec3 deltaRotation = newRotation - rocket->getRotation();

	physics->rotateRocket(deltaRotation);
}

void SimulationEngine::initialOrbitalInformation()
{
	simulationStopped = 0;
	lastAltitude = 0;
	apogeum = 0;
	perygeum = 0;
	lastAltitudeDirection = 1;
	altitudeDirection = 1;
}

void SimulationEngine::initWindowContext()
{
	int result = mainWindow->initialize();
	if (result != 0)
	{
		std::cout << "Main window creation failed. Status code = " << result << "\n";
	}		                                               
}

void SimulationEngine::switchGLStateForWorldRendering(float r, float g, float b)
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(r, g, b, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SimulationEngine::calcApogeumAndPerygeum()
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

void SimulationEngine::renderTelemetry(ofsim_gui::Gui* gui, Rocket* rocket, double altitude, double apogeum, double perygeum, double atmosphereDragForceMagnitude)
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

void SimulationEngine::syncFramerate(unsigned long long startTime, int ms_per_update)
{
	unsigned long long endTime = startTime + ms_per_update;
	while (currentTime() < endTime)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

SimulationEngine::~SimulationEngine()
{
	oddma->stop();		
	vm->stop();

	delete mainWindow;
	delete camera;

	for (int i = 0; i < renderables.size(); i++)
	{	
		delete renderables[i];
	}
		
	delete oddma;
	delete gui;
	delete vm;
	delete communicationBus;
	delete telemetryCollector;
	delete trajectoryPrediction;

	delete skyboxRenderer;
}

void keyPressedCallback(int keyPressed)
{
	lastKeyPressed = keyPressed;
}