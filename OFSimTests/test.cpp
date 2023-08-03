#include "pch.h"
#include <gtest/gtest.h>

#include "../../OFSim/src/vmachine/Memory.h"

TEST(OFSimTest, VMMemoryTest1) {
	Memory memory;

	unsigned int address{ 10 };

	unsigned char byteToStore{ 18 };
	memory.storeByte(address, byteToStore);

	unsigned char byteStored = memory.fetchByte(address);
	
	EXPECT_EQ(byteToStore, byteStored);
}

TEST(OFSimTest, VMMemoryTest2) {
	Memory memory;

	unsigned int address{ 12 };

	unsigned char byteToStore{ 17 };
	memory.storeByte(address, byteToStore);

	unsigned char byteStored = memory.fetchByte(address);

	EXPECT_EQ(byteToStore, byteStored);
}