#include "Instructions.h"

#include "../infrastructure/RocketCommand.h"

using namespace ofsim_vm;

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
	
	unsigned int value = memory.fetchWord(src_addr);
	registers[r_dst_addr] = value;
}

void Instructions::fld(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];

	unsigned int src_addr = registers[r_src_addr];

	double value = memory.fetchDWord(src_addr);
	registers.fl(r_dst_addr, value);
}

void Instructions::bld(unsigned char* args)
{
	unsigned char r_src_addr = args[1];
	unsigned char r_dst_addr = args[0];

	unsigned int src_addr = registers[r_src_addr];
	
	unsigned char value = memory.fetchByte(src_addr);
	registers[r_dst_addr] = value;
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

	double value = registers.fl(r_src_addr);
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
	int result = value2 - value1;
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
	double value1 = registers.fl(r_src_addr);
	double value2 = registers.fl(r_dst_addr);

	registers.zf(0);
	registers.cf(0);
	double result = value2 - value1;
	if (result == 0.0)
	{
		registers.zf(1);
	}
	else if (result < 0.0)
	{
		registers.cf(1); // r_src > r_dst arg[1] > arg[0]
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
	//unsigned int dst_addr = (registers.pc() + addr) % 0xffff;
	registers.pc(addr);
}

void Instructions::jmpr(unsigned char* args)
{
	unsigned char r_dst_addr = args[0];
	unsigned int addr = registers[r_dst_addr];
	//unsigned int dst_addr = (registers.pc() + addr) % 0xffff;
	registers.pc(addr);
}

void Instructions::halt(unsigned char* args)
{
	// do nthng!
}

void Instructions::cmd(unsigned char* args)
{
	// in first argument: register with instruction code
	// in second argument: floating point register with instruction value 
	unsigned char r_dst_addr = args[0];
	unsigned int cmd_code = registers[r_dst_addr];
	unsigned char r_src_addr = args[1];
	double value = registers.fl(r_src_addr);
	
	RocketCommand cmd((RocketCommandCode)cmd_code, value);
	eventProcessor.processVMCommand(cmd);
}

void ofsim_vm::Instructions::ftc(unsigned char* args)
{	
	dvec3 rotation = eventProcessor.getRocketRotation();
	double rotation_x = rotation.x;
	double rotation_y = rotation.y;
	double rotation_z = rotation.z;

	dvec3 position = eventProcessor.getRocketPosition();
	double position_x = position.x;
	double position_y = position.y;
	double position_z = position.z;
	
	dvec3 velocity = eventProcessor.getRocketVelocity();
	double velocity_x = velocity.x;
	double velocity_y = velocity.y;
	double velocity_z = velocity.z;

	double mass = eventProcessor.getRocketMass();
	double thrust_magnitude = eventProcessor.getThrustMagnitude();
	double altitude = eventProcessor.getAltitude();
	double runningTime = eventProcessor.getRunningTime();

	memory.storeDWord(65528, rotation_x);
	memory.storeDWord(65520, rotation_y);
	memory.storeDWord(65512, rotation_z);

	memory.storeDWord(65504, velocity_x);
	memory.storeDWord(65496, velocity_y);
	memory.storeDWord(65488, velocity_z);

	memory.storeDWord(65480, position_x);
	memory.storeDWord(65472, position_y);
	memory.storeDWord(65464, position_z);

	memory.storeDWord(65456, mass);
	memory.storeDWord(65448, thrust_magnitude);
	memory.storeDWord(65440, altitude);
	memory.storeDWord(65432, runningTime);
}

void Instructions::call(unsigned int opcode, unsigned char* args)
{
	switch (opcode)
	{
		case 0x0: mov(args); break;
		case 0x1: fmov(args); break;
		case 0x3: set(args); break;
		case 0x4: fset(args); break;
		case 0x5: ld(args); break;
		case 0x6: fld(args); break;
		case 0x7: bld(args); break;
		case 0x8: st(args); break;
		case 0x9: fst(args); break;
		case 0xA: bst(args); break;
		case 0xB: add(args); break;
		case 0xC: fadd(args); break;
		case 0xD: sub(args); break;
		case 0xE: fsub(args); break;
		case 0xF: mul(args); break;
		case 0x10: fmul(args); break;
		case 0x11: div(args); break;
		case 0x12: fdiv(args); break;
		case 0x13: mod(args); break;
		case 0x14: vor(args); break;
		case 0x15: vand(args); break;
		case 0x16: vxor(args); break;
		case 0x17: vnot(args); break;
		case 0x18: vshl(args); break;
		case 0x19: vshr(args); break;
		case 0x1A: cmp(args); break;
		case 0x1B: fcmp(args); break;
		case 0x1C: jz(args); break;
		case 0x1D: jnz(args); break;
		case 0x1E: jc(args); break;
		case 0x1F: jnc(args); break;
		case 0x20: jbe(args); break;
		case 0x21: ja(args); break;
		case 0x22: jmp(args); break;
		case 0x23: jmpr(args); break;
		case 0x24: halt(args); break;
		case 0x25: cmd(args); break;
		case 0x26: ftc(args); break;
	}
}
