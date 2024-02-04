#include <gtest/gtest.h>

#include <thread>
#include "../src/vmachine/Memory.h"

namespace vm_tests
{
	using namespace ofsim_vm;
	TEST(MemoryTests, simpleStoreAndFetchBytes)
	{
		Memory memory;

		memory.storeByte(0, 0x01);
		memory.storeByte(1, 0x02);
		memory.storeByte(2, 0x03);
		memory.storeByte(3, 0x04);

		unsigned char byte = memory.fetchByte(0);
		EXPECT_EQ(byte, 0x01);

		byte = memory.fetchByte(1);
		EXPECT_EQ(byte, 0x02);

		byte = memory.fetchByte(2);
		EXPECT_EQ(byte, 0x03);

		byte = memory.fetchByte(3);
		EXPECT_EQ(byte, 0x04);
	}

	TEST(MemoryTests, simpleStoreAndFetchWords)
	{
		Memory memory;

		memory.storeWord(0, 0x0102);
		memory.storeWord(2, 0x0304);
		memory.storeWord(4, 0x0506);
		memory.storeWord(6, 0x0708);

		unsigned short word = memory.fetchWord(0);
		EXPECT_EQ(word, 0x0102);

		word = memory.fetchWord(2);
		EXPECT_EQ(word, 0x0304);

		word = memory.fetchWord(4);
		EXPECT_EQ(word, 0x0506);

		word = memory.fetchWord(6);
		EXPECT_EQ(word, 0x0708);
	}

	TEST(MemoryTests, simpleStoreAndFetchDwords)
	{
		Memory memory;

		memory.storeDWord(0, 1234.123);
		memory.storeDWord(8, 456.567);
		memory.storeDWord(16, 890.1234);
		memory.storeDWord(24, 556677.12);

		double dword = memory.fetchDWord(0);
		EXPECT_EQ(dword, 1234.123);

		dword = memory.fetchDWord(8);
		EXPECT_EQ(dword, 456.567);

		dword = memory.fetchDWord(16);
		EXPECT_EQ(dword, 890.1234);

		dword = memory.fetchDWord(24);
		EXPECT_EQ(dword, 556677.12);
	}

	// write multithreaded tests of the Memory class

	TEST(MemoryTests, multithreadedStoreAndFetchBytes)
	{
		Memory memory;

		std::thread t1([&memory]() {
			memory.storeByte(0, 0x01);
			memory.storeByte(1, 0x02);
			memory.storeByte(2, 0x03);
			memory.storeByte(3, 0x04);
			});
		t1.join();

		unsigned char byte1{ 0 }, byte2{ 0 }, byte3{ 0 }, byte4{ 0 };
		std::thread t2([&memory, &byte1, &byte2, &byte3, &byte4]() {
			byte1 = memory.fetchByte(0);
			byte2 = memory.fetchByte(1);
			byte3 = memory.fetchByte(2);
			byte4 = memory.fetchByte(3);
			});

		t2.join();

		EXPECT_EQ(byte1, 0x01);
		EXPECT_EQ(byte2, 0x02);
		EXPECT_EQ(byte3, 0x03);
		EXPECT_EQ(byte4, 0x04);
	}
}