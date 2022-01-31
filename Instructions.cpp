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
