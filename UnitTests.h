#pragma once

#include "Memory.h"
#include "Registers.h"
#include "Instructions.h"
#include "VMachine.h"

class UnitTests
{
public:
	UnitTests() {}
	~UnitTests() {}
	void run();

private:
	void shouldTestVMMemory();	
	void shouldTestInstructions();
	void shouldTestSetInstructions();
	void shouldTestLoadAndStoreInstructions();

	void setup();
	void cleanUp();

	Memory* memory;
	Registers* registers;
	Instructions* instructions;
};