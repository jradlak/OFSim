#pragma once

#include "Registers.h"
#include "Memory.h"

class Instructions
{
public:
	Instructions(Memory& _memory, Registers& _registers)
		:memory(_memory), registers(_registers) {}
	
	// copy data operations:
	void mov(unsigned char* args);
	void fmov(unsigned char* args);

	void set(unsigned char* args);
	void fset(unsigned char* args);
	
	// load and store data from and to memory:
	void ld(unsigned char* args);
	void fld(unsigned char* args);
	void bld(unsigned char* args);

	void st(unsigned char* args);
	void fst(unsigned char* args);
	void bst(unsigned char* args);

	// arythmetical operations:
	void add(unsigned char* args);
	void fadd(unsigned char* args);
	void sub(unsigned char* args);
	void fsub(unsigned char* args);
	void mul(unsigned char* args);
	void fmul(unsigned char* args);
	void div(unsigned char* args);
	void fdiv(unsigned char* args);
	void mod(unsigned char* args);

	//logic operations:
	void vor(unsigned char* args);
	void vand(unsigned char* args);
	void vxor(unsigned char* args);	
	void vnot(unsigned char* args);
	void vshl(unsigned char* args);
	void vshr(unsigned char* args);

	// comparisions and conditional jumps:
	void cmp(unsigned char* args);
	void fcmp(unsigned char* args);

	void jz(unsigned char* args);
	void jnz(unsigned char* args);
	
	void jc(unsigned char* args);	
	void jnc(unsigned char* args);

	void jbe(unsigned char* args);
	void ja(unsigned char* args);

	// unconditional jumps:
	void jmp(unsigned char* args);  // jump to address
	void jmpr(unsigned char* args);  // jump to address from register

	// special instructions:
	void halt(unsigned char* args);

	void call(unsigned int opcode, unsigned char* args);

	~Instructions() {}
	
private:
	Memory& memory;
	Registers& registers;	
};