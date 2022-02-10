#pragma once

#include <iostream>

//#include "VMachine.h"

class Memory
{
public:
	Memory() { }

	unsigned char fetchByte(unsigned int addr);
	
	void storeByte(unsigned int addr, unsigned char byte);
	
	int fetchWord(unsigned int addr);
	
	void storeWord(unsigned int addr, int word);

	double fetchDWord(unsigned int addr);

	void storeDWord(unsigned int addr, double dword);

	static void memcopy(unsigned char* src, unsigned char* dst, unsigned int addr_s, unsigned int addr_d, unsigned int length);

	unsigned int size = 64 * 1024;
	unsigned char mem[64 * 1024] = { };

	~Memory() { }

private:
	
	void assertConditions(unsigned int addr);
	void blockSemaphore();
	void leaseSemaphore();

	bool blocked = false;
};