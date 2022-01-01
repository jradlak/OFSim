#include "Memory.h"

unsigned char Memory::fetchByte(unsigned int addr)
{
	assertKnown(addr);
	return mem[addr];
}

void Memory::storeByte(unsigned int addr, unsigned char byte)
{
	assertKnown(addr);
	mem[addr] = byte;
}

int Memory::fetchWord(unsigned int addr)
{
	assertKnown(addr + 4);
	unsigned char result[4] = { };
	memcopy(mem, result, addr, 0, 4);
	return *reinterpret_cast<int*>(result);
}

void Memory::storeWord(unsigned int addr, int word)
{
	assertKnown(addr + 4);
	unsigned char* result = static_cast<unsigned char*>(static_cast<void*>(&word));
	memcopy(result, mem, 0, addr, 4);
}

double Memory::fetchDWord(unsigned int addr)
{
	assertKnown(addr + 8);
	unsigned char result[8] = { };
	memcopy(mem, result, addr, 0, 8);
	return *reinterpret_cast<double*>(result);
}

void Memory::storeDWord(unsigned int addr, double dword)
{
	assertKnown(addr + 8);
	unsigned char* result = static_cast<unsigned char*>(static_cast<void*>(&dword));
	memcopy(result, mem, 0, addr, 8);
}

void Memory::memcopy(unsigned char* src, unsigned char* dst, unsigned int addr_s, unsigned int addr_d, unsigned int length)
{
	for (unsigned int i = 0; i < length; i++)
	{
		dst[addr_d + i] = src[addr_s + i];
	}
}

void Memory::assertKnown(unsigned int addr)
{
	if (addr > size)
	{
		throw 0x01; // memory address out of range
	}
}
