#pragma once

#include "Memory.h"
#include "Registers.h"
#include "Instructions.h"

class VMachine
{
public:
	VMachine();

	void interrupt(short code);

	~VMachine();

private:
	Memory* memory;
	Registers* registers;
	Instructions* instructions;

	void interruptHandler();
};