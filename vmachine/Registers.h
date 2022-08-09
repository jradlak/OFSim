#pragma once

#include <iostream>

class Registers
{
public:
	Registers() { }

	unsigned int& operator[](unsigned char index);

	double fl(unsigned char index);
	void fl(unsigned char index, double value);
	
	short cr();
	void cr(short value);
	
	short zf();
	void zf(short value);

	short cf();
	void cf(short value);

	unsigned int pc();
	void pc(unsigned int value);

	~Registers() { }

private:
	unsigned int size = 16;
	unsigned int r[16] = {};  // general purpose registers
	double flr[16] = {};      // floating point double precision registers	
	short creg = 0;			  // control register

	short zfreg = 0;		  // zero flag
	short cfreg = 0;		  // carry flag

	unsigned int pcreg = 0;   // program counter;

	void assertKnown(unsigned char index);
};
