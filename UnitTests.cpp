#include <iostream>

#include "UnitTests.h"
#include "Memory.h"

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
	Memory* mem = new Memory();

	int word = 3456;
	mem->storeWord(4, word);
	int word2 = 7865;
	mem->storeWord(12, word2);
	int result = mem->fetchWord(4);
	int result2 = mem->fetchWord(12);

	if (word != result || word2 != result2)
	{
		print(" ...failed");
		return;
	}

	print(" ...passed");
	delete mem;
}

void print(const char* text)
{
	std::cout << text << std::endl;
}
