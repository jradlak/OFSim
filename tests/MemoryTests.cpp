#include <gtest/gtest.h>

#include <thread>
#include <cstring>
#include <limits>
#include <stdexcept>
#include "../src/vmachine/Memory.h"

namespace vm_tests
{
using namespace ofsim_vm;

TEST(MemoryTests, LastCompleteRangesAndUnalignedValuesRoundTrip)
{
    Memory memory;
    memory.storeByte(mem_size - 1, 0xAB);
    EXPECT_EQ(memory.fetchByte(mem_size - 1), 0xAB);
    memory.storeWord(mem_size - 4, -1234567);
    EXPECT_EQ(memory.fetchWord(mem_size - 4), -1234567);
    memory.storeDWord(mem_size - 8, -1234.125);
    EXPECT_DOUBLE_EQ(memory.fetchDWord(mem_size - 8), -1234.125);

    memory.storeWord(1, -1234567);
    EXPECT_EQ(memory.fetchWord(1), -1234567);
    const int word = -1234567;
    EXPECT_EQ(std::memcmp(memory.mem + 1, &word, sizeof(word)), 0);
    memory.storeDWord(3, -1234.125);
    EXPECT_DOUBLE_EQ(memory.fetchDWord(3), -1234.125);
    const double dword = -1234.125;
    EXPECT_EQ(std::memcmp(memory.mem + 3, &dword, sizeof(dword)), 0);
}

TEST(MemoryTests, IncompleteRangesAndOverflowingAddressesAreRejectedWithoutWrites)
{
    Memory memory;
    const auto maxAddress = std::numeric_limits<unsigned int>::max();
    for (const auto addr : {mem_size, mem_size + 1, maxAddress})
    {
        EXPECT_THROW(memory.fetchByte(addr), std::out_of_range);
        EXPECT_THROW(memory.storeByte(addr, 42), std::out_of_range);
    }
    memory.storeDWord(mem_size - 8, 42.125);
    memory.storeDWord(0, 123.5);
    for (const auto addr : {mem_size - 3, mem_size - 1, mem_size, maxAddress - 3, maxAddress})
    {
        EXPECT_THROW(memory.fetchWord(addr), std::out_of_range);
        EXPECT_THROW(memory.storeWord(addr, 42), std::out_of_range);
    }
    for (const auto addr : {mem_size - 7, mem_size - 1, mem_size, maxAddress - 7, maxAddress})
    {
        EXPECT_THROW(memory.fetchDWord(addr), std::out_of_range);
        EXPECT_THROW(memory.storeDWord(addr, 42), std::out_of_range);
    }
    EXPECT_DOUBLE_EQ(memory.fetchDWord(0), 123.5);
    EXPECT_DOUBLE_EQ(memory.fetchDWord(mem_size - 8), 42.125);
}

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

    std::thread t1(
        [&memory]()
        {
            memory.storeByte(0, 0x01);
            memory.storeByte(1, 0x02);
            memory.storeByte(2, 0x03);
            memory.storeByte(3, 0x04);
        });
    t1.join();

    unsigned char byte1{0}, byte2{0}, byte3{0}, byte4{0};
    std::thread t2(
        [&memory, &byte1, &byte2, &byte3, &byte4]()
        {
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
} // namespace vm_tests
