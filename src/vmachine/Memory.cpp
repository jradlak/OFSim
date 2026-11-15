#include "Memory.h"

#include <cstring>
#include <stdexcept>
#include <string>

using namespace ofsim_vm;

static_assert(sizeof(int) == 4 && sizeof(double) == 8, "Unsupported VM data sizes");

unsigned char Memory::fetchByte(unsigned int addr)
{
    std::lock_guard<std::mutex> lock(memMutex);
    assertRange(addr, 1);
    char val = mem[addr];
    return val;
}

void Memory::storeByte(unsigned int addr, unsigned char byte)
{
    std::lock_guard<std::mutex> lock(memMutex);
    assertRange(addr, 1);
    mem[addr] = byte;
}

int Memory::fetchWord(unsigned int addr)
{
    std::lock_guard<std::mutex> lock(memMutex);
    assertRange(addr, sizeof(int));
    int val;
    std::memcpy(&val, mem + addr, sizeof(val));
    return val;
}

void Memory::storeWord(unsigned int addr, int word)
{
    std::lock_guard<std::mutex> lock(memMutex);
    assertRange(addr, sizeof(word));
    std::memcpy(mem + addr, &word, sizeof(word));
}

double Memory::fetchDWord(unsigned int addr)
{
    std::lock_guard<std::mutex> lock(memMutex);
    assertRange(addr, sizeof(double));
    double val;
    std::memcpy(&val, mem + addr, sizeof(val));
    return val;
}

void Memory::storeDWord(unsigned int addr, double dword)
{
    std::lock_guard<std::mutex> lock(memMutex);
    assertRange(addr, sizeof(dword));
    std::memcpy(mem + addr, &dword, sizeof(dword));
}

void Memory::memcopy(unsigned char* src, unsigned char* dst, unsigned int addr_s,
                     unsigned int addr_d, unsigned int length)
{
    for (unsigned int i = 0; i < length; i++)
    {
        dst[addr_d + i] = src[addr_s + i];
    }
}

void Memory::clear()
{
    std::lock_guard<std::mutex> lock(memMutex);
    for (int i = 0; i < mem_size; i++)
    {
        mem[i] = 0;
    }
}

void Memory::assertRange(unsigned int addr, unsigned int length) const
{
    if (addr > mem_size || length > mem_size - addr)
    {
        throw std::out_of_range("VM memory access out of range: address " + std::to_string(addr) +
                                ", size " + std::to_string(length));
    }
}
