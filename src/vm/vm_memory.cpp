#include "vmachine.h"

namespace vm
{
	unsigned char fetch_byte(Tmemory& memory, unsigned int addr)
	{
		std::lock_guard<std::mutex> lock(memory.mem_mutex);
		assert_conditions(addr);
		char val = memory.mem[addr];
		return val;
	}

	void store_byte(Tmemory& memory,unsigned int addr, unsigned char byte)
	{
		std::lock_guard<std::mutex> lock(memory.mem_mutex);
		assert_conditions(addr);
		memory.mem[addr] = byte;
	}

	int fetch_word(Tmemory& memory, unsigned int addr)
	{
		std::lock_guard<std::mutex> lock(memory.mem_mutex);
		assert_conditions(addr + 4);
		unsigned char result[4] = { };
		memcopy(memory.mem, result, addr, 0, 4);
		unsigned int val = *reinterpret_cast<unsigned int*>(result);
		return val;
	}

	void store_word(Tmemory& memory, unsigned int addr, int word)
	{
		std::lock_guard<std::mutex> lock(memory.mem_mutex);
		assert_conditions(addr + 4);
		unsigned char* result = static_cast<unsigned char*>(static_cast<void*>(&word));
		memcopy(result, memory.mem, 0, addr, 4);
	}

	double fetch_dword(Tmemory& memory, unsigned int addr)
	{
		std::lock_guard<std::mutex> lock(memory.mem_mutex);
		assert_conditions(addr + 8);
		unsigned char result[8] = { };
		memcopy(memory.mem, result, addr, 0, 8);
		double val = *reinterpret_cast<double*>(result);
		return val;
	}

	void store_dword(Tmemory& memory, unsigned int addr, double dword)
	{
		std::lock_guard<std::mutex> lock(memory.mem_mutex);
		assert_conditions(addr + 8);
		unsigned char* result
			= static_cast<unsigned char*>(static_cast<void*>(&dword));
		memcopy(result, memory.mem, 0, addr, 8);
	}

	void memcopy(unsigned char* src, unsigned char* dst, unsigned int addr_s, unsigned int addr_d, unsigned int length)
	{
		for (unsigned int i = 0; i < length; i++)
		{
			dst[addr_d + i] = src[addr_s + i];
		}
	}

	void clear(Tmemory& memory)
	{
		std::lock_guard<std::mutex> lock(memory.mem_mutex);
		for (int i = 0; i < memory_size; i++) 
		{
			memory.mem[i] = 0;
		}
	}

	void assert_conditions(unsigned int addr)
	{
		if (addr > memory_size)
		{
			throw 0x01; // memory address out of range
		}
	}
}