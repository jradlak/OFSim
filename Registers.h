#pragma once

#include <iostream>

class Registers
{
public:
	Registers() { }

	unsigned int& operator[](int index)
	{
		assertKnown(index);
		return r[index];
	}

	double fl(unsigned int index)
	{
		assertKnown(index);
		return flr[index];
	}

	void fl(unsigned int index, double value)
	{
		assertKnown(index);
		flr[index] = value;
	}

	short fr()
	{
		return freg;
	}

	void fr(short value)
	{
		freg = value;
	}

	short cr()
	{
		return creg;
	}

	void cr(short value)
	{
		creg = value;
	}

	~Registers() { }

private:
	unsigned int size = 16;
	unsigned int r[16] = {};  // general purpose registers
	double flr[16] = {};      // floating point double precision registers
	short freg = 0;			  // state register
	short creg = 0;			  // control register

	void assertKnown(unsigned int index)
	{
		if (index > size)
		{
			std::cout << "Unknown register!!! Program must be terminated!" << std::endl;
			exit(0);
		}
	}
};
