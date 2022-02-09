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

	~Translator() { delete opcodes; }

private:
    unsigned char code[64 * 512] = { }; // code buffer
	unsigned int instr_addr = 0;
	std::map<std::string, unsigned int> labelDict;
	Opcodes* opcodes;

	std::tuple<unsigned int, unsigned int> recognizeInstr(std::string sourceLine);
	void translate(std::string sourceLine);
	void findLabel(std::string sourceLine);

    void trnsl_mov(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_fmov(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_set(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_fset(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_ld(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_fld(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_bld(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_st(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_fst(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_bst(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_add(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_fadd(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_sub(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_fsub(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_mul(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_fmul(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_div(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_fdiv(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_mod(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_vor(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_vand(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_vxor(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_vnot(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_vshl(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_vshr(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_cmp(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_fcmp(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_jz(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_jnz(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_jc(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_jnc(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_jbe(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_ja(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_jmp(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_jmpr(std::tuple<unsigned int, unsigned int> instr, std::string line);
    void trnsl_halt(std::tuple<unsigned int, unsigned int> instr, std::string line);
};