#include <iostream>

#include "UnitTests.h"
#include "Memory.h"
#include "VMachine.h"

void print(const char* text);

void UnitTests::run()
{
	shouldTestVMMemory();

	int wait;
	std::cin >> wait;
}

void UnitTests::shouldTestVMMemory()
{
	print("shouldTestVMMemory testing... ");
	//VMachine* vm = new VMachine();
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
		return;
	}

	print(" ...passed");
	delete mem;
	//delete vm;
}

void print(const char* text)
{
	std::cout << text << std::endl;
}
