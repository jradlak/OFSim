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
	VMachine();

	void interrupt(short code);

	void interpret(const char* sourcePath);

	Memory* getMemory() { return memory; }

	~VMachine();

private:
	Memory* memory;
	Registers* registers;
	Opcodes* opcodes;
	Instructions* instructions;
	Translator* translator;

	void interruptHandler();
};