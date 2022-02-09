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

void Instructions::add(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];
	unsigned int value1 = registers[r_src_addr];
	unsigned int value2 = registers[r_dst_addr];

	registers[r_dst_addr] = value1 + value2;
}

void Instructions::fadd(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];
	double value1 = registers.fl(r_src_addr);
	double value2 = registers.fl(r_dst_addr);

	registers.fl(r_dst_addr, value1 + value2);
}

void Instructions::sub(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];
	unsigned int value1 = registers[r_src_addr];
	unsigned int value2 = registers[r_dst_addr];

	registers[r_dst_addr] = value2 - value1;
}

void Instructions::fsub(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];
	double value1 = registers.fl(r_src_addr);
	double value2 = registers.fl(r_dst_addr);

	registers.fl(r_dst_addr, value2 - value1);
}

void Instructions::mul(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];
	unsigned int value1 = registers[r_src_addr];
	unsigned int value2 = registers[r_dst_addr];

	registers[r_dst_addr] = value1 * value2;
}

void Instructions::fmul(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];
	double value1 = registers.fl(r_src_addr);
	double value2 = registers.fl(r_dst_addr);

	registers.fl(r_dst_addr, value1 * value2);
}

void Instructions::div(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];
	unsigned int value1 = registers[r_src_addr];
	unsigned int value2 = registers[r_dst_addr];

	registers[r_dst_addr] = value2 / value1;
}

void Instructions::fdiv(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];
	double value1 = registers.fl(r_src_addr);
	double value2 = registers.fl(r_dst_addr);

	registers.fl(r_dst_addr, value2 / value1);
}

void Instructions::mod(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];
	unsigned int value1 = registers[r_src_addr];
	unsigned int value2 = registers[r_dst_addr];

	registers[r_dst_addr] = value2 % value1;
}

void Instructions::vor(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];
	unsigned int value1 = registers[r_src_addr];
	unsigned int value2 = registers[r_dst_addr];

	registers[r_dst_addr] = value1 | value2;
}

void Instructions::vand(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];
	unsigned int value1 = registers[r_src_addr];
	unsigned int value2 = registers[r_dst_addr];

	registers[r_dst_addr] = value1 & value2;
}

void Instructions::vxor(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];
	unsigned int value1 = registers[r_src_addr];
	unsigned int value2 = registers[r_dst_addr];

	registers[r_dst_addr] = value1 ^ value2;
}

void Instructions::vnot(unsigned char* args)
{
	unsigned char r_src_addr = args[1];	
	unsigned int value1 = registers[r_src_addr];	

	registers[r_src_addr] = ~value1;
}

void Instructions::vshl(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];
	unsigned int value1 = registers[r_src_addr];
	unsigned int value2 = registers[r_dst_addr];

	registers[r_dst_addr] = value2 << value1;
}

void Instructions::vshr(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];
	unsigned int value1 = registers[r_src_addr];
	unsigned int value2 = registers[r_dst_addr];

	registers[r_dst_addr] = value2 >> value1;
}

void Instructions::cmp(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];
	unsigned int value1 = registers[r_src_addr];
	unsigned int value2 = registers[r_dst_addr];

	registers.zf(0);
	registers.cf(0);
	int result = value1 - value1;
	if (result == 0) 
	{
		registers.zf(1);
	} 
	else if (result < 0)
	{
		registers.cf(1);
	}
}

void Instructions::fcmp(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];
	double value1 = registers[r_src_addr];
	double value2 = registers[r_dst_addr];

	registers.zf(0);
	registers.cf(0);
	double result = value1 - value1;
	if (result == 0.0)
	{
		registers.zf(1);
	}
	else if (result < 0.0)
	{
		registers.cf(1);
	}

}

void Instructions::jz(unsigned char* args)
{
	if (registers.zf() == 1)
	{
		jmp(args);
	}
}

void Instructions::jnz(unsigned char* args)
{
	if (registers.zf() == 0)
	{
		jmp(args);
	}
}

void Instructions::jc(unsigned char* args)
{
	if (registers.cf() == 1)
	{
		jmp(args);
	}
}

void Instructions::jnc(unsigned char* args)
{
	if (registers.cf() == 0)
	{
		jmp(args);
	}
}

void Instructions::jbe(unsigned char* args)
{
	if (registers.zf() == 1 || registers.cf() == 1)
	{
		jmp(args);
	}
}

void Instructions::ja(unsigned char* args)
{
	if (registers.cf() == 1 && registers.zf() == 0)
	{
		jmp(args);
	}
}

void Instructions::jmp(unsigned char* args)
{
	unsigned char aaddr[4] = { };
	Memory::memcopy(args, aaddr, 0, 0, 4);
	unsigned int addr = *reinterpret_cast<unsigned int*>(aaddr);
	unsigned int dst_addr = (registers.pc() + addr) % 0xffff;
	registers.pc(dst_addr);
}

void Instructions::jmpr(unsigned char* args)
{
	unsigned char r_dst_addr = args[0];
	unsigned int addr = registers[r_dst_addr];
	unsigned int dst_addr = (registers.pc() + addr) % 0xffff;
	registers.pc(dst_addr);
}

void Instructions::call(unsigned int opcode, unsigned char* args)
{
	switch (opcode)
	{
	case 0x0: mov(args); break;
	case 0x1: fmov(args); break;
	}
}
