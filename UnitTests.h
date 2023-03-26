#pragma once

#include "src\vmachine\Memory.h"
#include "src\vmachine\Registers.h"
#include "src\vmachine\Instructions.h"
#include "src\vmachine\VMachine.h"
#include "src\vmachine\Translator.h"
#include "src\vmachine\Opcodes.h"

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

	CommunicationBus* commandBus;
};