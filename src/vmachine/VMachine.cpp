#include "VMachine.h"

using namespace ofsim_vm;

VMachine::VMachine(com_bus::Tbus_data* commandBus)
{
	memory = std::make_unique<Memory>();
	registers = std::make_unique<Registers>();
	opcodes = std::make_unique<Opcodes>();
	instructions = std::make_unique<Instructions>(*memory, *registers, *commandBus);
	translator = std::make_unique<Translator>();
}

void VMachine::translateSourceCode(const char* _sourcePath)
{
	sourcePath = _sourcePath;	
	translator->translateSourceFile(_sourcePath);

	// load translated code into memory:
	unsigned int codeSize = translator->getCodeSize();
	Memory::memcopy(translator->code, memory->mem, 0, 0, codeSize);
}

void VMachine::executionLoop()
{
	// code execution:		
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
}

void VMachine::stop()
{
	shouldStop = true;
	takeANap();	
	registers->clear();
	memory->clear();
}

void VMachine::restart()
{
	stop();
	takeANap();	
	start();
}

void VMachine::start()
{	
	takeANap();
	shouldStop = false;
	translateSourceCode(sourcePath);	
}

void VMachine::takeANap()
{	
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
