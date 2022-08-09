#pragma once

#include "vmachine\Memory.h"
#include "vmachine\Registers.h"
#include "vmachine\Instructions.h"
#include "vmachine\VMachine.h"
#include "vmachine\Translator.h"
#include "vmachine\Opcodes.h"

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
	void shouldTestVM();

	void setup();
	void cleanUp();

	Memory* memory;
	Registers* registers;
	Instructions* instructions;


	Opcodes* opcodes;
	Translator* translator;
	VMachine* vm;
};