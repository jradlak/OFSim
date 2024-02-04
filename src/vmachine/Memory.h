#pragma once

#include <iostream>
#include <mutex>

namespace ofsim_vm
{

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

	constexpr unsigned int mem_size = 64 * 1024;	

	class Memory
	{
	public:		
		unsigned char mem[mem_size] = { };

		Memory() { }

		static void memcopy(unsigned char* src, unsigned char* dst, unsigned int addr_s, unsigned int addr_d, unsigned int length);

		unsigned char fetchByte(unsigned int addr);

		void storeByte(unsigned int addr, unsigned char byte);

		int fetchWord(unsigned int addr);

		void storeWord(unsigned int addr, int word);

		double fetchDWord(unsigned int addr);

		void storeDWord(unsigned int addr, double dword);

		void clear();

		~Memory() { }

	private:
		std::mutex memMutex;
		void assertConditions(unsigned int addr) const;
	};
} // namespace ofsim_vm