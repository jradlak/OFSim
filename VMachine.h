#pragma once

#include "Memory.h"
#include "Registers.h"

class VMachine
{
public:
	VMachine();

	void interrupt(short code);

	~VMachine();

private:
	Memory* memory;
	Registers* registers;	

	void interruptHandler();
};