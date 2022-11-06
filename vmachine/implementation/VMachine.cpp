#pragma once

#include "..\VMachine.h"

VMachine::VMachine(CommunicationBus* commandBus)
{
	memory = new Memory();
	registers = new Registers();
	opcodes = new Opcodes();
	instructions = new Instructions(*memory, *registers, *commandBus);
	translator = new Translator();
}

void VMachine::translateSourceCode(const char* _sourcePath)
{
	sourcePath = _sourcePath;	
	translator->translate(_sourcePath);

	// load translated code into memory:
	unsigned int codeSize = translator->getCodeSize();
	Memory::memcopy(translator->code, memory->mem, 0, 0, codeSize);
}

void VMachine::executionLoop()
{
	// code execution:	
	threadFinished = 0;
	unsigned int opcode = memory->fetchByte(0);
	while (opcode != opcodes->getOpcode("halt") && !shouldStop)
	{
		while (pause); // wait

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
	}

	threadFinished = 1;
}

void VMachine::waitUntilExecutionFinished()
{
	while (threadFinished == 0);
}

void VMachine::stop()
{
	shouldStop = true;
	waitUntilExecutionFinished();
	takeANap();	
	registers->clear();
	memory->clear();
}

void VMachine::setPause()
{
	pause = true;
}

void VMachine::unPause()
{
	pause = false;
}

void VMachine::restart()
{
	stop();
	takeANap();	
	start();
}

void VMachine::init()
{
}

void VMachine::start()
{	
	takeANap();
	shouldStop = false;
	translateSourceCode(sourcePath);
	std::thread executionLoopThred = std::thread(&VMachine::executionLoop, this);
	executionLoopThred.detach();		
}

void VMachine::takeANap()
{	
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

VMachine::~VMachine()
{
	delete translator;
	delete instructions;
	delete opcodes;
	delete memory;
	delete registers;	
}
