#include "Memory.h"

unsigned char Memory::fetchByte(unsigned int addr)
{
	std::lock_guard<std::mutex> lock(memMutex);
	assertConditions(addr);
	char val = mem[addr];		
	return val;
}

void Memory::storeByte(unsigned int addr, unsigned char byte)
{
	std::lock_guard<std::mutex> lock(memMutex);
	assertConditions(addr);
	mem[addr] = byte;	
}

int Memory::fetchWord(unsigned int addr)
{
	std::lock_guard<std::mutex> lock(memMutex);
	assertConditions(addr + 4);
	unsigned char result[4] = { };
	memcopy(mem, result, addr, 0, 4);
	unsigned int val = *reinterpret_cast<unsigned int*>(result);	
	return val;
}

void Memory::storeWord(unsigned int addr, int word)
{
	std::lock_guard<std::mutex> lock(memMutex);
	assertConditions(addr + 4);
	unsigned char* result = static_cast<unsigned char*>(static_cast<void*>(&word));
	memcopy(result, mem, 0, addr, 4);	
}
	
double Memory::fetchDWord(unsigned int addr)
{
	std::lock_guard<std::mutex> lock(memMutex);
	assertConditions(addr + 8);
	unsigned char result[8] = { };
	memcopy(mem, result, addr, 0, 8);
	double val = *reinterpret_cast<double*>(result);	
	return val;
}

void Memory::storeDWord(unsigned int addr, double dword)
{
	std::lock_guard<std::mutex> lock(memMutex);
	assertConditions(addr + 8);
	unsigned char* result 
		= static_cast<unsigned char*>(static_cast<void*>(&dword));
	memcopy(result, mem, 0, addr, 8);	
}

void Memory::memcopy(unsigned char* src, unsigned char* dst, unsigned int addr_s, unsigned int addr_d, unsigned int length)
{	
	for (unsigned int i = 0; i < length; i++)
	{
		dst[addr_d + i] = src[addr_s + i];
	}
}

void Memory::clear()
{
	std::lock_guard<std::mutex> lock(memMutex);
	for (int i = 0; i < size; i++) {
		mem[i] = 0;
	}
}

void Memory::assertConditions(unsigned int addr) const
{
	if (addr > size)
	{
		throw 0x01; // memory address out of range
	}
}
