#pragma once

#include "Memory.h"
#include "Registers.h"
#include "Instructions.h"
#include "VMachine.h"
#include "Translator.h"

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

	void shouldTestTranslator();

	void setup();
	void cleanUp();

	Memory* memory;
	Registers* registers;
	Instructions* instructions;

	Translator* translator;
};