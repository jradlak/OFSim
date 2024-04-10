#pragma once

#include <vector>
#include <tuple>
#include "Instructions.h"

namespace ofsim_vm
{
	class Opcodes
	{
	public:
		Opcodes()
		{
			opcodes.push_back(std::make_tuple(0x0, "mov", 0x2));
			opcodes.push_back(std::make_tuple(0x1, "fmov", 0x2));
			opcodes.push_back(std::make_tuple(0x3, "set", 0x5));
			opcodes.push_back(std::make_tuple(0x4, "fset", 0x9));

			opcodes.push_back(std::make_tuple(0x5, "ld", 0x2));
			opcodes.push_back(std::make_tuple(0x6, "fld", 0x2));
			opcodes.push_back(std::make_tuple(0x7, "bld", 0x2));
			opcodes.push_back(std::make_tuple(0x8, "st", 0x2));
			opcodes.push_back(std::make_tuple(0x9, "fst", 0x2));
			opcodes.push_back(std::make_tuple(0xA, "bst", 0x2));

			opcodes.push_back(std::make_tuple(0xB, "add", 0x2));
			opcodes.push_back(std::make_tuple(0xC, "fadd", 0x2));
			opcodes.push_back(std::make_tuple(0xD, "sub", 0x2));
			opcodes.push_back(std::make_tuple(0xE, "fsub", 0x2));
			opcodes.push_back(std::make_tuple(0xF, "mul", 0x2));
			opcodes.push_back(std::make_tuple(0x10, "fmul", 0x2));
			opcodes.push_back(std::make_tuple(0x11, "div", 0x2));
			opcodes.push_back(std::make_tuple(0x12, "fdiv", 0x2));
			opcodes.push_back(std::make_tuple(0x13, "mod", 0x2));
			opcodes.push_back(std::make_tuple(0x14, "vor", 0x2));
			opcodes.push_back(std::make_tuple(0x15, "vand", 0x2));
			opcodes.push_back(std::make_tuple(0x16, "vxor", 0x2));
			opcodes.push_back(std::make_tuple(0x17, "vnot", 0x1));
			opcodes.push_back(std::make_tuple(0x18, "vshl", 0x2));
			opcodes.push_back(std::make_tuple(0x19, "vshr", 0x2));

			opcodes.push_back(std::make_tuple(0x1A, "cmp", 0x2));
			opcodes.push_back(std::make_tuple(0x1B, "fcmp", 0x2));
			opcodes.push_back(std::make_tuple(0x1C, "jz", 0x4));
			opcodes.push_back(std::make_tuple(0x1D, "jnz", 0x4));
			opcodes.push_back(std::make_tuple(0x1E, "jc", 0x4));
			opcodes.push_back(std::make_tuple(0x1F, "jnc", 0x4));
			opcodes.push_back(std::make_tuple(0x20, "jbe", 0x4));
			opcodes.push_back(std::make_tuple(0x21, "ja", 0x4));

			opcodes.push_back(std::make_tuple(0x22, "jmp", 0x4));
			opcodes.push_back(std::make_tuple(0x23, "jmpr", 0x1));
			opcodes.push_back(std::make_tuple(0x24, "halt", 0x0));
			opcodes.push_back(std::make_tuple(0x25, "cmd", 0x2));
			opcodes.push_back(std::make_tuple(0x26, "ftc", 0x0));
		}

		unsigned int getOpcode(std::string name)
		{
			for (int i = 0; i < opcodes.size(); i++)
			{
				if (std::get<1>(opcodes[i]) == name)
				{
					return std::get<0>(opcodes[i]);
				}
			}

			return 0x100;
		}

		std::string getInstrName(unsigned int opcode)
		{
			for (int i = 0; i < opcodes.size(); i++)
			{
				if (std::get<0>(opcodes[i]) == opcode)
				{
					return std::get<1>(opcodes[i]);
				}
			}

			return "";
		}

		unsigned int getInstrSize(unsigned int opcode)
		{
			for (int i = 0; i < opcodes.size(); i++)
			{
				if (std::get<0>(opcodes[i]) == opcode)
				{
					return std::get<2>(opcodes[i]);
				}
			}

			return 0x0;
		}

		bool isJump(unsigned int opcode)
		{
			return opcode >= 0x1C && opcode != 0x24;
		}

		~Opcodes() {}

	private:
		std::vector<std::tuple<unsigned int, std::string, unsigned int>> opcodes;
	};
}