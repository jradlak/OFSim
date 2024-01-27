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
		std::map<unsigned int, std::string> op {};
		std::map<unsigned int, unsigned int> op_size {};
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