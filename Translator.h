#pragma once

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <tuple>

#include "Opcodes.h"

class Translator
{
public:
	Translator() { opcodes = new Opcodes(); }

	void translate(const char* sourcePath);
   
    unsigned int getCodeSize() { return instr_addr; }

    unsigned char code[64 * 512] = { }; // code buffer

	~Translator() { delete opcodes; }

private:   
	unsigned int instr_addr = 0;
	std::map<std::string, unsigned int> labelDict;
	Opcodes* opcodes;

	std::tuple<unsigned int, unsigned int> recognizeInstr(std::string sourceLine);
	void translate(std::string sourceLine);
	void findLabel(std::string sourceLine);

    void trnsl_constant_to_register(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_register_to_register(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_constant(std::tuple<unsigned int, unsigned int> instr, std::string line);
	void trnsl_register(std::tuple<unsigned int, unsigned int> instr, std::string line);
	void trnsl_halt(std::tuple<unsigned int, unsigned int> instr, std::string line);
};