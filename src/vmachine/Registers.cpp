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

short Registers::cr()
{
	return creg;
}

void Registers::cr(short value)
{
	creg = value;
}

short Registers::zf()
{
	return zfreg;
}

void Registers::zf(short value)
{
	zfreg = value;
}

short Registers::cf()
{
	return cfreg;
}

void Registers::cf(short value)
{
	cfreg = value;
}

unsigned int Registers::pc()
{
	return pcreg;
}

void Registers::pc(unsigned int value)
{
	pcreg = value;
}

void Registers::clear()
{
	for (int i = 0; i < 16; i++) {
		r[i] = 0;
		flr[i] = 0;		
	}

	creg = 0;
	cfreg = 0;
	zfreg = 0;
	pcreg = 0;
}

void Registers::assertKnown(unsigned char index)
{
	if (index > size)
	{
		std::cout << "Unknown register!!! Program must be terminated!" << std::endl;
		exit(0);
	}
}