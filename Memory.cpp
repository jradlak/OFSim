#include "Memory.h"

unsigned char Memory::fetchByte(unsigned int addr)
{
	assertConditions(addr);
	char val = mem[addr];
	leaseSemaphore();
	return val;
}

void Memory::storeByte(unsigned int addr, unsigned char byte)
{
	assertConditions(addr);
	mem[addr] = byte;
	leaseSemaphore();
}

int Memory::fetchWord(unsigned int addr)
{
	assertConditions(addr + 4);
	unsigned char result[4] = { };
	memcopy(mem, result, addr, 0, 4);
	int val = *reinterpret_cast<int*>(result);
	leaseSemaphore();
	return val;
}

void Memory::storeWord(unsigned int addr, int word)
{
	assertConditions(addr + 4);
	unsigned char* result = static_cast<unsigned char*>(static_cast<void*>(&word));
	memcopy(result, mem, 0, addr, 4);
	leaseSemaphore();
}
	
double Memory::fetchDWord(unsigned int addr)
{
	assertConditions(addr + 8);
	unsigned char result[8] = { };
	memcopy(mem, result, addr, 0, 8);
	double val = *reinterpret_cast<double*>(result);
	leaseSemaphore();
	return val;
}

void Memory::storeDWord(unsigned int addr, double dword)
{
	assertConditions(addr + 8);
	unsigned char* result = static_cast<unsigned char*>(static_cast<void*>(&dword));
	memcopy(result, mem, 0, addr, 8);
	leaseSemaphore();
}

void Memory::memcopy(unsigned char* src, unsigned char* dst, unsigned int addr_s, unsigned int addr_d, unsigned int length)
{
	for (unsigned int i = 0; i < length; i++)
	{
		dst[addr_d + i] = src[addr_s + i];
	}
}

void Memory::assertConditions(unsigned int addr)
{
	if (addr > size)
	{
		throw 0x01; // memory address out of range
	}

	while (blocked) 
	{
		// wait!
	}

	blockSemaphore();
}

void Memory::blockSemaphore()
{
	blocked = true;
}

void Memory::leaseSemaphore()
{
	blocked = false;
}
