#pragma once

#include "Registers.h"
#include "Memory.h"

class Instructions
{
public:
	Instructions(Memory& _memory, Registers& _registers)
		:memory(_memory), registers(_registers) {}
		
	void mov(unsigned char* args);
	void fmov(unsigned char* args);

	void set(unsigned char* args);
	void fset(unsigned char* args);

	~Instructions() {}
	
private:
	Memory& memory;
	Registers& registers;	
};