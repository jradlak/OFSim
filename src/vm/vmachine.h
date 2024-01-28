#pragma once

#include <map>
#include <string>
#include <mutex>

#include "../infrastructure/CommunicationBus.h"

namespace vm 
{
	constexpr unsigned int memory_size{ 65536 };
	constexpr unsigned int num_registers{ 16 };
	
	struct Tmemory
	{
		// flat array of 64 kilobytes:
		unsigned char mem[memory_size] { };
		std::mutex mem_mutex;
	};

	struct Tregisters
	{		
		unsigned int r[num_registers] {};   // general purpose registers
		double fl[num_registers] {};	    // floating point double precision registers	
		short cr{ 0 };						// control register

		short zf{ 0 };						// zero flag
		short cf{ 0 };						// carry flag

		unsigned int pc{ 0 };				// program counter;
	};

	struct Topcodes
	{
		std::map<unsigned int, std::string> op 
		{
			// move and set registers operations:
			{0x0, "mov"}, {0x1, "fmov"},
			{0x3, "set"}, {0x4, "fset"},
			
			// load and store data from and to memory:
			{0x5, "ld"}, {0x6, "fld"},
			{0x7, "bld"}, {0x8, "st"},
			{0x9, "fst"}, {0xA, "bst"},
			
			// math and logic operations:
			{0xB, "add"}, {0xC, "fadd"},
			{0xD, "sub"}, {0xE, "fsub"},
			{0xF, "mul"}, {0x10, "fmul"}
			{0x11, "div"}, {0x12, "fdiv"},
			{0x13, "mod"}, {0x14, "vor"},
			{0x15, "vand"}, {0x16, "vxor"},
			{0x17, "vnot"}, {0x18, "vshl"},
			{0x19, "vshr"},
			
			// comparison and jump instructions:
			{0x1A, "cmp"}, {0x1B, "fcmp"},
			{0x1C, "jz"}, {0x1D, "jnz"},
			{0x1E, "jc"}, {0x1F, "jnc"},
			{0x20, "jbe"}, {0x21, "ja"},

			// unconditional jumps:			
			{0x22, "jmp"}, {0x23, "jmpr"},

			// special instructions:
			{0x24, "halt"}, {0x25, "cmd"}
		};

		std::map<unsigned int, unsigned int> op_size 
		{
			{0x0, 0x2}, {0x1, 0x2},
			{0x3, 0x5}, {0x4, 0x9},
						
			{0x5, 0x2}, {0x6, 0x2},
			{0x7, 0x2}, {0x8, 0x2},
			{0x9, 0x2}, {0xA, 0x2},
						
			{0xB, 0x2}, {0xC, 0x2},
			{0xD, 0x2}, {0xE, 0x2},
			{0xF, 0x2}, {0x10, 0x2}
			{0x11, 0x2}, {0x12, 0x2},
			{0x13, 0x2}, {0x14, 0x2},
			{0x15, 0x2}, {0x16, 0x2},
			{0x17, 0x1}, {0x18, 0x2},
			{0x19, 0x2},
						
			{0x1A, 0x2}, {0x1B, 0x2},
			{0x1C, 0x4}, {0x1D, 0x4},
			{0x1E, 0x4}, {0x1F, 0x4},
			{0x20, 0x4}, {0x21, 0x4},
			
			{0x22, 0x4}, {0x23, 0x1},
			
			{0x24, 0x0}, {0x25, 0x2}
		};
	};
	
	struct Tvm
	{
		Tmemory memory;
		Tregisters registers;
		Topcodes opcodes;
		com_bus::Tbus_data& commandBus;
	};

	// memory operations:
	void memcopy(unsigned char* src, unsigned char* dst, unsigned int addr_s, unsigned int addr_d, unsigned int length);
	unsigned char fetch_byte(Tmemory& memory, unsigned int addr);
	void store_byte(Tmemory& memory, unsigned int addr, unsigned char byte);
	int fetch_word(Tmemory& memory, unsigned int addr);
	void store_word(Tmemory& memory, unsigned int addr, int word);
	double fetch_dword(Tmemory& memory, unsigned int addr);
	void store_dword(Tmemory& memory, unsigned int addr, double dword);
	void clear(Tmemory& memory);
	void assert_conditions(unsigned int addr);

	// registers operations:
	void clear(Tregisters& registers);

	// instructions:

} // namespace vm