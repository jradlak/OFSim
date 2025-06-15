#include "VMachine.h"

using namespace ofsim_vm;

VMachine::VMachine(ofsim_events::EventProcessor& _event_processor) : event_processor(_event_processor)
{	
}

std::string VMachine::translateSourceCodeFromFile(std::string _sourcePath)
{
	source_path = _sourcePath;	
	std::string source_code = translator.translateSourceFile(_sourcePath);
    load_code();

	return source_code;
}

void ofsim_vm::VMachine::translate_source_code(std::string sourceCode)
{
	translator.translateSourceString(sourceCode.c_str());
	load_code();
}

void ofsim_vm::VMachine::load_code()
{
	memory.clear(); // clear old program before load new one
	Memory::memcopy(translator.code, memory.mem, 0, 0, translator.getCodeSize());
}

void VMachine::executionLoop()
{
	// code execution:		
	unsigned int opcode = memory.fetchByte(0);
	while (opcode != opcodes.getOpcode("halt") && !should_stop)
	{
		// decode and execute instruction:
		unsigned int args_size = opcodes.getInstrSize(opcode);
		unsigned char* args = new unsigned char[args_size];
		Memory::memcopy(memory.mem, args, pc + 1, 0, args_size);
		instructions.call(opcode, args);
		delete[] args;		
		pc = registers.pc();
		if (oldpc == pc)
		{
			// there was no jump - update program counter:			
			pc += args_size + 1;
			registers.pc(pc);
		}
		oldpc = pc;

		// fetch another opcode
		opcode = memory.fetchByte(pc);
	}	
}

void VMachine::stop()
{
	should_stop = true;
	started = false;
	take_a_nap();	
	registers.clear();
	memory.clear();
	pc = 0;
	oldpc = 0;
}

void VMachine::restart()
{
	stop();
	take_a_nap();	
	start();
}

void VMachine::start()
{	
	take_a_nap();
	should_stop = false;
	started = true;
	std::cout << "VMachine execution loop started!\n";	
	executionLoop();
}
