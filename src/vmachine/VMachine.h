#pragma once

#include <chrono>
#include <thread>

#include "Memory.h"
#include "Registers.h"
#include "Instructions.h"
#include "Translator.h"
#include "Opcodes.h"

#include "../simulation/Task.h"

#include "../math_and_physics/MathTypes.h"

class VMachine : public Task
{
public:
	VMachine(com_bus::Tbus_data* commandBus);
	
	void translateSourceCode(const char* _sourcePath);
	
	// Task methods:
	
	void init() {}

	void start();

	void stop();

	void setPause() { pause = true; }

	void unPause() { pause = false; }

	void restart();
	
	///////////////////

	void provideSourcePath(const char* _sourcePath) { sourcePath = _sourcePath; }

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

	u32 pc = 0;
	u32 oldpc = 0;

	bool shouldStop = true;
	bool pause = false;		
};