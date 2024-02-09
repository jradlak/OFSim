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
#include <regex>
#include <memory>

#include "Opcodes.h"
#include "../gui/i18n.h"

namespace ofsim_vm
{
	class Translator
	{
	public:
		Translator();

		void translateSourceFile(const char* sourcePath);
		void translateSourceString(const std::string sourceString);
		void translateLine(std::string sourceLine, int lineNumber);

		unsigned int getCodeSize() { return instr_addr; }

		std::string getTranslationErrors() { return translationErrors; }
		void clearTranslationErrors() { translationErrors = ""; }

		unsigned char code[64 * 512] { }; // code buffer

		~Translator() { }

	private:
		std::unique_ptr<Opcodes> opcodes;

		unsigned int instr_addr{ 0 };
		ofsim_gui::I18n* i18n{ nullptr };
		std::string translationErrors{ "" };

		std::map<std::string, unsigned int> labelDict;

		std::tuple<unsigned int, unsigned int> recognizeInstr(std::string sourceLine);

		void findLabel(std::string sourceLine);

		void trnsl_constant_to_register(std::tuple<unsigned int, unsigned int> instr, std::string line);
		void trnsl_fconstant_to_register(std::tuple<unsigned int, unsigned int> instr, std::string line);
		void trnsl_register_to_register(std::tuple<unsigned int, unsigned int> instr, std::string line);
		void trnsl_constant(std::tuple<unsigned int, unsigned int> instr, std::string line);
		void trnsl_register(std::tuple<unsigned int, unsigned int> instr, std::string line);
		void trnsl_halt(std::tuple<unsigned int, unsigned int> instr, std::string line);
	};
}