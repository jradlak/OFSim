#pragma once

#include <chrono>
#include <memory>

#include "Memory.h"
#include "Registers.h"
#include "Instructions.h"
#include "Translator.h"
#include "Opcodes.h"

#include "../math_and_physics/MathTypes.h"

class VMachine
{
public:
	VMachine(com_bus::Tbus_data* commandBus);
	
	void translateSourceCode(const char* _sourcePath);
		
	void start();

	void stop();

	void setPause() { pause = true; }

	void unPause() { pause = false; }

	void restart();
		
	void provideSourcePath(const char* _sourcePath) { sourcePath = _sourcePath; }

	void takeANap();

	Memory* getMemory() { return memory.get(); }
		
	~VMachine() {}

private:
	void executionLoop();

	std::unique_ptr<Memory> memory;
	std::unique_ptr<Registers> registers;
	std::unique_ptr<Opcodes> opcodes;
	std::unique_ptr<Instructions> instructions;
	std::unique_ptr<Translator> translator;

	const char* sourcePath = nullptr;

	u32 pc = 0;
	u32 oldpc = 0;

	bool shouldStop = true;
	bool pause = false;		
};