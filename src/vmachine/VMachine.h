#pragma once

#include <chrono>
#include <thread>

#include "Memory.h"
#include "Registers.h"
#include "Instructions.h"
#include "Translator.h"
#include "Opcodes.h"

#include "../simulation/Task.h"

class VMachine : public Task
{
public:
	VMachine(CommunicationBus* commandBus);
	
	void translateSourceCode(const char* _sourcePath);
	
	// Task methods:
	
	void init();

	void start();

	void stop();

	void setPause();

	void unPause();

	void restart();
	
	///////////////////

	void provideSourcePath(const char* _sourcePath) { sourcePath = _sourcePath; }

	void takeANap();

	Memory* getMemory() { return memory; }
		
	~VMachine();

private:

	void executionLoop();
	void waitUntilExecutionFinished();

	Memory* memory;
	Registers* registers;
	Opcodes* opcodes;
	Instructions* instructions;
	Translator* translator;

	const char* sourcePath;

	unsigned int pc = 0;
	unsigned int oldpc = 0;

	bool shouldStop = true;
	bool pause = false;	

	bool threadFinished = 1;
};