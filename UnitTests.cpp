#include <iostream>

#include "UnitTests.h"

void print(const char* text);

void UnitTests::run()
{
	shouldTestVMMemory();
	shouldTestInstructions();
	shouldTestSetInstructions();
	shouldTestLoadAndStoreInstructions();

	shouldTestTranslator();

	int wait;
	std::cin >> wait;
}

void UnitTests::shouldTestVMMemory()
{
	print("shouldTestVMMemory testing... ");
	Memory* mem = new Memory();

	int word = 3456;
	int word2 = 7865;
	double dword1 = 123.456;
	double dword2 = 5678.9876;

	mem->storeWord(4, word);	
	mem->storeWord(12, word2);
	mem->storeDWord(1024, dword1);
	mem->storeDWord(1156, dword2);

	int result = mem->fetchWord(4);
	int result2 = mem->fetchWord(12);
	double result3 = mem->fetchDWord(1024);
	double result4 = mem->fetchDWord(1156);

	bool wasException = false;
	try
	{
		int word = mem->fetchWord(70000);
	}
	catch (int e)
	{
		if (e == 0x01) 
		{
			wasException = true;
		}
	}

	if (word != result || word2 != result2 || 
		dword1 != result3 || dword2 != result4 ||
		!wasException)
	{
		print(" ...failed");
		delete mem;
		return;
	}

	print(" ...passed");
	delete mem;
	//delete vm;
}

void UnitTests::shouldTestInstructions()
{
	print("shouldTestInstructions testing... ");
	setup();

	registers->operator[](1) = 0;
	registers->operator[](2) = 20;

	//mov:
	unsigned char args[] = { 1, 2 };
	instructions->mov(args);


	if (registers->operator[](1) != 20) 
	{
		print(" ...failed");

		cleanUp();
		return;
	}

	print(" ...passed");
	cleanUp();
}

void UnitTests::shouldTestSetInstructions()
{
	print("shouldTestSetInstructions testing... ");
	setup();

	// test regular word:
	unsigned char args[5] = { };
	args[0] = 3; // sets third register
	unsigned int value = 135711;
	unsigned char* rvalue = static_cast<unsigned char*>(static_cast<void*>(&value));
	Memory::memcopy(rvalue, args, 0, 1, 4);

	instructions->set(args);

	// test floating point number:
	unsigned char fargs[9] = { };
	fargs[0] = 4; // sets fourth floating point register
	double fvalue = 1357.11;
	unsigned char* rfvalue = static_cast<unsigned char*>(static_cast<void*>(&fvalue));
	Memory::memcopy(rfvalue, fargs, 0, 1, 8);

	instructions->fset(fargs);

	if (registers->operator[](3) != value ||
		registers->fl(4) != fvalue)
	{
		print(" ...failed");

		cleanUp();
		return;
	}

	print(" ...passed");
	cleanUp();
}

void UnitTests::shouldTestLoadAndStoreInstructions()
{
	print("shouldTestLoadAndStoreInstructions testing... ");
	setup();

	unsigned int src_addr = 13;
	unsigned int dst_addr = 16;

	int word = 3456;

	//ld:
	memory->storeWord(src_addr, word);
	registers->operator[](8) = src_addr;
	registers->operator[](10) = dst_addr;

	unsigned char args[] = { 10, 8 };
	instructions->ld(args);

	if (memory->fetchWord(dst_addr) != word) {
		print(" ...failed");

		cleanUp();
		return;
	}

	print(" ...passed");
	cleanUp();
}

void UnitTests::shouldTestTranslator()
{
	print("shouldTestTranslator testing... ");
	setup();

	translator->translate("orbitalProgram1.oasm");

	print(" ...passed");
	cleanUp();
}

void UnitTests::setup()
{
	memory = new Memory();
	registers = new Registers();
	instructions = new Instructions(*memory, *registers);

	translator = new Translator();
}

void UnitTests::cleanUp()
{
	delete translator;

	delete instructions;
	delete registers;
	delete memory;
}

void print(const char* text)
{
	std::cout << text << std::endl;
}
