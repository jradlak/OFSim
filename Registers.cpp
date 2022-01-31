#include "Registers.h"

unsigned int& Registers::operator[](unsigned char index)
{
	assertKnown(index);
	return r[index];
}

double Registers::fl(unsigned char index)
{
	assertKnown(index);
	return flr[index];
}

void Registers::fl(unsigned char index, double value)
{
	assertKnown(index);
	flr[index] = value;
}

short Registers::fr()
{
	return freg;
}

void Registers::fr(short value)
{
	freg = value;
}

short Registers::cr()
{
	return creg;
}

void Registers::cr(short value)
{
	creg = value;
}

void Registers::assertKnown(unsigned char index)
{
	if (index > size)
	{
		std::cout << "Unknown register!!! Program must be terminated!" << std::endl;
		exit(0);
	}
}