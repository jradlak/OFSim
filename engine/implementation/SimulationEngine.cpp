#include "..\SimulationEngine.h"

#include "../world/SolarSystemConstants.h"

SimulationEngine::SimulationEngine()
{
	camera = new Camera(glm::vec3(-100.0, -160.0, 1000.0));
	mainWindow = new Window(*camera, SCR_WIDTH, SCR_HEIGHT);
	initWindowContext();

	SolarSystem* solarSystem = new SolarSystem();
	
	// rocket:
	glm::dvec3 rocketPos = solarSystem->pointAboveEarthSurface(angle, dangle, -0.2);
	rocket = new Rocket("model3d_shader", rocketPos, 0.000013);
	
	camera->position = rocket->getPosition() + glm::dvec3(0.0, 0.024, 0.0);
	
	// physics:
	glm::dvec3 towards = solarSystem->pointAboveEarthSurface(angle, dangle, -50.0);
	physics = new PhysicsEngine(*rocket, MS_PER_UPDATE);
	physics->changeAltitudeOrientation(CelestialBodyType::planet, 3185.0, towards);

	// register renderables:
	renderables.push_back(solarSystem);
	renderables.push_back(rocket);
}

void SimulationEngine::init()
{
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
}

