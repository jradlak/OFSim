#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "..\SimulationEngine.h"

#include "../world/SolarSystemConstants.h"

static char orbitalProgramSourceCode[1024 * 16];

SimulationEngine::SimulationEngine()
{
	camera = new Camera(glm::vec3(-100.0, -160.0, 1000.0));
	mainWindow = new Window(*camera, SCR_WIDTH, SCR_HEIGHT);
	initWindowContext();

	solarSystem = new SolarSystem();
	
	// rocket:
	glm::dvec3 rocketPos = solarSystem->pointAboveEarthSurface(angle, dangle, -0.2);
	rocket = new Rocket("model3d_shader", rocketPos, 0.000013);
	
	camera->position = rocket->getPosition() + glm::dvec3(0.0, 0.024, 0.0);
	
	// physics:
	glm::dvec3 towards = solarSystem->pointAboveEarthSurface(angle, dangle, -50.0);
	physics = new PhysicsEngine(*rocket, MS_PER_UPDATE);
	physics->changeAltitudeOrientation(CelestialBodyType::planet, 3185.0, towards);

	// initialize communication Bus:
	communicationBus = new CommunicationBus();

	// initialize Virtual Machine:
	vm = new VMachine(communicationBus);
	vm->translateSourceCode("orbital_programs/ballisticProgram.oasm");
	vm->start();

	// initialize and start ODDMA:
	oddma = new ODDMA(rocket, physics, vm, communicationBus);
	oddma->start();

	// initialize GUI:
	createGui();
	loadSourceCode("orbital_programs/ballisticProgram.oasm");

	// init time variables:
	startTime = currentTime();
	runningTime = 0;
	timePaused = 0;

	initialPhysicsInformation();
	initialOrbitalInformation();

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
}

void SimulationEngine::stop()
{
}

void SimulationEngine::restart()
{
}

void SimulationEngine::mainLoop()
{
	while (!mainWindow->shouldClose())
	{

		// camera/view transformation:
		camera->updatePosition(rocket->getPosition(), rocket->getRotation());
		camera->processCameraRotation(3.0, 0);
		glm::dmat4 view = camera->getViewMatrix();

		// render renderables:
		for (int i = 0; i < renderables.size(); i++)
		{
			renderables[i]->render(projection, view, SolarSystemConstants::lightPos);
		}
	}
}

unsigned __int64 SimulationEngine::currentTime()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
		).count();
}

void SimulationEngine::createGui()
{
	gui = new Gui();
	gui->initialization(mainWindow);
	gui->loadButtonTextures();
}

void SimulationEngine::loadSourceCode(std::string sourcePath)
{
	std::ifstream sourceFile;

	std::string sourceCode = "";

	sourceFile.open(sourcePath.c_str(), std::ios::in);

	if (sourceFile.is_open()) {
		std::string line;

		while (sourceFile)
		{
			std::getline(sourceFile, line, '\r');
			sourceFile >> line;
			sourceCode += line + "\n";
		}

		sourceFile.close();
	}

	static char* srcStr = (char*)sourceCode.c_str();	
	strcpy(orbitalProgramSourceCode, srcStr);
}

void SimulationEngine::initialPhysicsInformation()
{
	glm::dvec3 newRotation = glm::dvec3(-50.000021, 48.8000050, 0.0);
	glm::dvec3 deltaRotation = newRotation - rocket->getRotation();
	
	physics->rotateRocket(deltaRotation);
	solarSystem->provideRocketInformation(angle, dangle, rocket);
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

SimulationEngine::~SimulationEngine()
{
	delete mainWindow;
	delete camera;

	for (int i = 0; i < renderables.size(); i++)
	{	
		delete renderables[i];
	}

	delete vm;
	delete communicationBus;
	delete oddma;
	delete gui;
}

