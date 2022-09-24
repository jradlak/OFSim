#pragma once

#include <chrono>
#include <thread>

#include "Memory.h"
#include "Registers.h"
#include "Instructions.h"
#include "Translator.h"
#include "Opcodes.h"

class VMachine
{
public:
	VMachine(CommunicationBus* commandBus);

	void interrupt(short code);

	void interpret(const char* _sourcePath);

	void terminate();
	
	void setPause(bool _pause) { pause = _pause; }

	void reset();

	Memory* getMemory() { return memory; }
	
	~VMachine();

private:
	Memory* memory;
	Registers* registers;
	Opcodes* opcodes;
	Instructions* instructions;
	Translator* translator;

	const char* sourcePath;

	bool shouldStop = false;
	bool pause = true;

	void interruptHandler();
	void takeANap();
};