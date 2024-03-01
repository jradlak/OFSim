#include "Registers.h"

using namespace ofsim_vm;

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

void Registers::clear()
{
	for (int i = 0; i < number_of_registers; i++) {
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
	if (index > number_of_registers)
	{
		std::cout << "Unknown register!!! Program must be terminated!" << std::endl;
		exit(0);
	}
}