#pragma once

#include <iostream>

class Registers
{
public:
	Registers() { }

	unsigned int& operator[](int index);

	double fl(unsigned int index);

	void fl(unsigned int index, double value);

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

	void assertKnown(unsigned int index);
};
