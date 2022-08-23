#pragma once

#include "..\VMachine.h"

VMachine::VMachine(CommandBus* commandBus)
{
	memory = new Memory();
	registers = new Registers();
	opcodes = new Opcodes();
	instructions = new Instructions(*memory, *registers, *commandBus);
	translator = new Translator();
}

void VMachine::interrupt(short code)
{
}

void VMachine::interpret(const char* sourcePath)
{
	shouldStop = false;
	translator->translate(sourcePath);

	// load translated code into memory:
	unsigned int codeSize = translator->getCodeSize();
	Memory::memcopy(translator->code, memory->mem, 0, 0, codeSize);

	// code execution:
	unsigned int pc = 0;
	unsigned int oldpc = 0;
	unsigned int opcode = memory->fetchByte(0);
	while (opcode != opcodes->getOpcode("halt") && !shouldStop)
	{
		// decode and execute instruction:
		unsigned int args_size = opcodes->getInstrSize(opcode);
		unsigned char* args = new unsigned char[args_size];
		Memory::memcopy(memory->mem, args, pc + 1, 0, args_size);
		instructions->call(opcode, args);
		delete[] args;

		pc = registers->pc();
		if (oldpc == pc)
		{
			// there was no jump - update program counter:			
			pc += args_size + 1;
			registers->pc(pc);
		}
		oldpc = pc;

		// fetch another opcode
		opcode = memory->fetchByte(pc);

		//std::this_thread::sleep_for(std::chrono::microseconds(1));
	}
}

void VMachine::terminate()
{
	shouldStop = true;
}

VMachine::~VMachine()
{
	delete translator;
	delete instructions;
	delete opcodes;
	delete memory;
	delete registers;	
}

void VMachine::interruptHandler()
{
}
