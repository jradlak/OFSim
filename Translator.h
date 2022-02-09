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

class Translator
{
public:
	Translator() { }

	void translate(const char* sourcePath);

	~Translator() { }
private:
	std::vector<unsigned char> code;
	int line_addr = 0;
	std::map<std::string, unsigned int> labelDict;

	void translate(std::string sourceLine);
	void findLabel(std::string sourceLine);
};