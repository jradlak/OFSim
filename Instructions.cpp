#include "Instructions.h"

void Instructions::mov(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];

	unsigned int value = registers[r_src_addr];
	registers[r_dst_addr] = value;
}

void Instructions::fmov(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];

	double value = registers.fl(r_src_addr);
	registers.fl(r_dst_addr, value);
}

void Instructions::set(unsigned char* args)
{
	unsigned char r_dst_addr = args[0];
	unsigned char avalue[4] = { };
	Memory::memcopy(args, avalue, 1, 0, 4);
	unsigned int rvalue = *reinterpret_cast<unsigned int*>(avalue);
	registers[r_dst_addr] = rvalue;
}

void Instructions::fset(unsigned char* args)
{
	unsigned char r_dst_addr = args[0];
	unsigned char avalue[8] = { };
	Memory::memcopy(args, avalue, 1, 0, 8);
	double rvalue = *reinterpret_cast<double*>(avalue);
	registers.fl(r_dst_addr, rvalue);
}

void Instructions::ld(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];

	unsigned int src_addr = registers[r_src_addr];
	unsigned int dst_addr = registers[r_dst_addr];
	
	unsigned int value = memory.fetchWord(src_addr);
	memory.storeWord(dst_addr, value);
}

void Instructions::fld(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];

	unsigned int src_addr = registers[r_src_addr];
	unsigned int dst_addr = registers[r_dst_addr];

	double value = memory.fetchDWord(src_addr);
	memory.storeDWord(dst_addr, value);
}

void Instructions::bld(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];

	unsigned int src_addr = registers[r_src_addr];
	unsigned int dst_addr = registers[r_dst_addr];

	unsigned char value = memory.fetchByte(src_addr);
	memory.storeByte(dst_addr, value);
}

void Instructions::st(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];

	unsigned int value = registers[r_src_addr];
	unsigned int dst_addr = registers[r_dst_addr];
	memory.storeWord(dst_addr, value);
}

void Instructions::fst(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];

	double value = registers[r_src_addr];
	unsigned int dst_addr = registers[r_dst_addr];
	memory.storeDWord(dst_addr, value);
}

void Instructions::bst(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];

	unsigned char value = registers[r_src_addr] & 0xFF;
	unsigned int dst_addr = registers[r_dst_addr];
	memory.storeByte(dst_addr, value);
}
