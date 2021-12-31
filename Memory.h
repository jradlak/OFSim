#pragma once

#include <iostream>

class Memory
{
public:
	Memory()
	{

	}

	unsigned char fetchByte(unsigned int addr)
	{
		assertKnown(addr);
		return mem[addr];
	}

	int fetchWord(unsigned int addr)
	{
		assertKnown(addr + 4);
		unsigned char result[4] = { };
		memcopy(mem, result, addr, 0, 4);
		return *reinterpret_cast<int*>(result);
	}

	void storeWord(unsigned int addr, int word)
	{
		assertKnown(addr + 4);
		unsigned char* result = static_cast<unsigned char*>(static_cast<void*>(&word));
		memcopy(result, mem, 0, addr, 4);
	}

	void memcopy(unsigned char* src, unsigned char* dst, unsigned int addr_s, unsigned int addr_d, unsigned int length)
	{
		for (unsigned int i = 0; i < length; i++)
		{
			dst[addr_d + i] = src[addr_s + i];
		}
	}

	~Memory()
	{

	}

private:
	unsigned int size = 64 * 1024;
	unsigned char mem[64 * 1024] = {};

	void assertKnown(unsigned int addr)
	{
		if (addr > size)
		{
			std::cout << "Unknown address!!! Program must be terminated!" << std::endl;
			exit(0);
		}
	}
};