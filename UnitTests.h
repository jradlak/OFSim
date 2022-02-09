#pragma once

#include "Memory.h"
#include "Registers.h"
#include "Instructions.h"
#include "VMachine.h"
#include "Translator.h"
#include "Opcodes.h"

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

	void shouldTestOpcodes();

	void shouldTestTranslator();

	void setup();
	void cleanUp();

	Memory* memory;
	Registers* registers;
	Instructions* instructions;

	Opcodes* opcodes;
	Translator* translator;
};