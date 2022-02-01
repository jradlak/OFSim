#pragma once

#include "Registers.h"
#include "Memory.h"

class Instructions
{
public:
	Instructions(Memory& _memory, Registers& _registers)
		:memory(_memory), registers(_registers) {}
		

	// copy data instructions:

	void mov(unsigned char* args);
	void fmov(unsigned char* args);

	void set(unsigned char* args);
	void fset(unsigned char* args);

	// copy 32 bit word from memory from address pointed by registry r_src to 
	// to address pointed by registry r_dsc.
	// ld r_dst, r_src
	void ld(unsigned char* args);
	
	void fld(unsigned char* args);
	void bld(unsigned char* args);

	void st(unsigned char* args);
	void fst(unsigned char* args);
	void bst(unsigned char* args);

	~Instructions() {}
	
private:
	Memory& memory;
	Registers& registers;	
};