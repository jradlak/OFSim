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

	void interpret(const char* sourcePath);

	void terminate();
	
	void setPause(bool _pause) { pause = _pause; }

	Memory* getMemory() { return memory; }
	
	~VMachine();

private:
	Memory* memory;
	Registers* registers;
	Opcodes* opcodes;
	Instructions* instructions;
	Translator* translator;

	bool shouldStop = false;
	bool pause = true;

	void interruptHandler();
};