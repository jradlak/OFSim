#pragma once

#include <chrono>
#include <thread>

#include "Memory.h"
#include "Registers.h"
#include "Instructions.h"
#include "Translator.h"
#include "Opcodes.h"

#include "../engine/Task.h"

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

	void takeANap();

	Memory* getMemory() { return memory; }
	
	~VMachine();

private:

	void executionLoop();

	Memory* memory;
	Registers* registers;
	Opcodes* opcodes;
	Instructions* instructions;
	Translator* translator;

	const char* sourcePath;

	bool shouldStop = false;
	bool pause = true;	
};