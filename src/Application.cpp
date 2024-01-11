#include "simulation/SimulationEngine.h"

int main(int argc, char** argv)
{
	SimulationEngine* simulation = new SimulationEngine();
	simulation->init();
	simulation->start();
	simulation->stop();

	delete simulation;
}