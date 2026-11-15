#pragma once

#include <iostream>
#include <mutex>
#include <span>

namespace ofsim_vm
{

// memory structure:
/*
Rotation:
    z	65528
    y	65520
    x	65512
Velocity:
    z	65504
    y	65496
    x	65488
Position:
    z	65480
    y	65472
    x	65464

    mass	65456
    thrust magnitude	65448
    altitude	65440
    timestamp	65432
*/

constexpr unsigned int mem_size = 64 * 1024; // 64 KB

class Memory
{
public:
    Memory()
    {
        clear();
    }

    void readBlock(unsigned int addr, std::span<unsigned char> destination);
    void writeBlock(unsigned int addr, std::span<const unsigned char> source);

    unsigned char fetchByte(unsigned int addr);

    void storeByte(unsigned int addr, unsigned char byte);

    int fetchWord(unsigned int addr);

    void storeWord(unsigned int addr, int word);

    double fetchDWord(unsigned int addr);

    void storeDWord(unsigned int addr, double dword);

    void clear();

private:
    unsigned char mem[mem_size]{0};
    std::mutex memMutex;
    void assertRange(unsigned int addr, std::size_t length) const;
};
} // namespace ofsim_vm
