#pragma once

#include <iostream>

//#include "VMachine.h"

// memory structure:
/*
Rotation:
	z	65528
	y	65520
	x	65512
Velocity:	
	z	65504
	y	65496
	x	65488
Position:	
	z	65480
	y	65472
	x	65464
	
	mass	65456
	thrust magnitude	65448
	altitude	65440
	timestamp	65432

*/


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

	void clear();

	unsigned int size = 64 * 1024;
	unsigned char mem[64 * 1024] = { };

	~Memory() { }

private:
	
	void assertConditions(unsigned int addr);
	void blockSemaphore();
	void leaseSemaphore();

	bool blocked = false;
};