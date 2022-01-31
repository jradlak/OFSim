#pragma once

#include <iostream>

class Registers
{
public:
	Registers() { }

	unsigned int& operator[](unsigned char index);

	double fl(unsigned char index);

	void fl(unsigned char index, double value);

	short fr();

	void fr(short value);

	short cr();

	void cr(short value);

	~Registers() { }

private:
	unsigned int size = 16;
	unsigned int r[16] = {};  // general purpose registers
	double flr[16] = {};      // floating point double precision registers
	short freg = 0;			  // state register
	short creg = 0;			  // control register

	void assertKnown(unsigned char index);
};
