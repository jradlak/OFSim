#pragma once

#include <iostream>

namespace ofsim_vm
{
	class Registers
	{
	public:
		Registers() { }

		unsigned int& operator[](unsigned char index);

		double fl(unsigned char index);
		void fl(unsigned char index, double value);

		short cr() const { return creg; }
		void cr(short value) { creg = value; }

		short zf() const { return zfreg; }
		void zf(short value) { zfreg = value; }

		short cf() const { return cfreg; }
		void cf(short value) { cfreg = value; }

		unsigned int pc() const { return pcreg; }
		void pc(unsigned int value) { pcreg = value; }

		void clear();

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
}