#include <gtest/gtest.h>

#include <stdexcept>
#include <limits>

#include "../src/infrastructure/EventProcessor.h"
#include "../src/vmachine/VMachine.h"

namespace vm_tests
{
using namespace ofsim_vm;

TEST(RegistersTests, FirstAndLastRegistersAreAccessible)
{
    Registers registers;
    for (const unsigned char index : {0, 15})
    {
        registers[index] = 123456;
        registers.fl(index, -123.5);
        EXPECT_EQ(registers[index], 123456);
        EXPECT_DOUBLE_EQ(registers.fl(index), -123.5);
    }
}

TEST(RegistersTests, InvalidIndicesAreRejectedForIntegerAndFloatingPointRegisters)
{
    Registers registers;
    registers[15] = 42;
    registers.fl(0, 123.5);
    for (const unsigned char index : {16, 17, 255})
    {
        EXPECT_THROW(registers[index], std::out_of_range);
        EXPECT_THROW(registers[index] = 123, std::out_of_range);
        EXPECT_THROW(registers.fl(index), std::out_of_range);
        EXPECT_THROW(registers.fl(index, 123.5), std::out_of_range);
    }
    EXPECT_EQ(registers[15], 42);
    EXPECT_DOUBLE_EQ(registers.fl(0), 123.5);
}

std::string hello_world_vm = R"(
; store address of data in register (Hello world literal)
set r4, data

; set up registers for memory addresses
vxor r0, r0
set r1, 1
set r3, 256

print_loop:
  ; fetch byte from addres stored in r4
  bld r2, r4

  ; if zero, exit the loop
  cmp r2, r0
jz .end

; otherwise store byte in desired address in memory
bst r3, r2
add r3, r1

; move r4 on another character and go back
; to the beginning of the loop
add r4, r1
jmp print_loop

.end:
  halt
data:
  db Hello world)";

TEST(VMachineTests, simpleRun)
{
    // Arrange
    ofsim_events::EventProcessor* eventProcessor = ofsim_events::EventProcessor::getInstance();
    VMachine vm(*eventProcessor);
    vm.translateSourceCode(hello_world_vm);

    // Act
    vm.start();

    // Assert
    EXPECT_EQ(vm.getMemory()->fetchByte(255), 0);
    EXPECT_EQ(vm.getMemory()->fetchByte(256), 72);
    EXPECT_EQ(vm.getMemory()->fetchByte(257), 101);
}
} // namespace vm_tests

namespace vm_tests
{
TEST(VMachineTests, EmptyFailedAndClearedProgramsCannotExecuteOldCode)
{
    VMachine vm(*ofsim_events::EventProcessor::getInstance());
    vm.translateSourceCode("halt");
    EXPECT_THROW(vm.translateSourceCode("; empty"), std::runtime_error);
    EXPECT_THROW(vm.start(), std::out_of_range);
    EXPECT_FALSE(vm.isStarted());
    vm.translateSourceCode("halt");
    EXPECT_THROW(vm.translateSourceCode("jmp missing"), std::runtime_error);
    EXPECT_THROW(vm.start(), std::out_of_range);
    EXPECT_EQ(vm.getMemory()->fetchByte(0), 0);
    vm.translateSourceCode("halt");
    EXPECT_NO_THROW(vm.start());
    vm.clearMemory();
    EXPECT_THROW(vm.start(), std::out_of_range);
}

TEST(VMachineTests, UnknownOpcodeAndIncompleteInstructionsAreRejected)
{
    VMachine vm(*ofsim_events::EventProcessor::getInstance());
    vm.translateSourceCode("halt");
    vm.getMemory()->storeByte(0, 0x02);
    EXPECT_THROW(vm.start(), std::invalid_argument);
    EXPECT_FALSE(vm.isStarted());
    for (const unsigned char opcode : {0x03, 0x04, 0x22, 0x23})
    {
        vm.translateSourceCode("halt");
        vm.getMemory()->storeByte(0, opcode);
        EXPECT_THROW(vm.start(), std::out_of_range);
    }
    // The missing operand would still fit in VM memory, but lies outside the image.
    vm.translateSourceCode("db abcdefghi");
    vm.getMemory()->storeByte(0, 0x04);
    EXPECT_THROW(vm.start(), std::out_of_range);
}

TEST(VMachineTests, InvalidSecondRegisterIsRejectedDuringDecoding)
{
    VMachine vm(*ofsim_events::EventProcessor::getInstance());
    vm.translateSourceCode("mov r0, r1\nhalt");
    vm.getMemory()->storeByte(2, 16);
    EXPECT_THROW(vm.start(), std::out_of_range);
}

TEST(VMachineTests, FallingOffTheImageAndOutOfImageJumpsReportErrors)
{
    VMachine vm(*ofsim_events::EventProcessor::getInstance());
    for (const std::string program :
         {"jmp 5", "jmp 65535\nhalt", "jmp 4294967295\nhalt", "set r0, -1\njmpr r0\nhalt",
          "set r0, 65536\njmpr r0\nhalt", "ftc", "set r0, 42"})
    {
        SCOPED_TRACE(program);
        vm.translateSourceCode(program);
        EXPECT_THROW(vm.start(), std::out_of_range);
        EXPECT_FALSE(vm.isStarted());
    }
}

TEST(VMachineTests, DataAccessRemainsAvailableBeyondTheLoadedImage)
{
    VMachine vm(*ofsim_events::EventProcessor::getInstance());
    vm.translateSourceCode("set r0, 65535\nset r1, 42\nbst r0, r1\nbld r2, r0\n"
                           "set r0, 256\nbst r0, r2\nhalt");
    EXPECT_NO_THROW(vm.start());
    EXPECT_EQ(vm.getMemory()->fetchByte(65535), 42);
    EXPECT_EQ(vm.getMemory()->fetchByte(256), 42);
}

TEST(VMachineTests, UntakenJumpFallsThroughAndNoArgumentInstructionsRemainValid)
{
    VMachine vm(*ofsim_events::EventProcessor::getInstance());
    vm.translateSourceCode("set r0, 256\nset r1, 42\ncmp r1, r1\njnz bad\n"
                           "bst r0, r1\nftc\nhalt\nbad: db X");
    EXPECT_NO_THROW(vm.start());
    EXPECT_EQ(vm.getMemory()->fetchByte(256), 42);
}

TEST(VMachineTests, ModifiedCodeIsValidatedWhenFetched)
{
    VMachine vm(*ofsim_events::EventProcessor::getInstance());
    vm.translateSourceCode("set r0, next\nset r1, 2\nbst r0, r1\nnext: halt");
    EXPECT_THROW(vm.start(), std::invalid_argument);
}

TEST(VMachineTests, RestartReloadsTheProgramAndResetsItsCounter)
{
    VMachine vm(*ofsim_events::EventProcessor::getInstance());
    vm.translateSourceCode("set r0, 256\nset r1, 42\nbst r0, r1\nhalt");
    vm.start();
    vm.getMemory()->storeByte(0, 2);
    vm.getMemory()->storeByte(256, 99);
    EXPECT_NO_THROW(vm.restart());
    EXPECT_EQ(vm.getMemory()->fetchByte(256), 42);
}
} // namespace vm_tests

namespace vm_tests
{
TEST(VMachineTests, InstructionAtTheLastByteOfAFullImageCanHalt)
{
    VMachine vm(*ofsim_events::EventProcessor::getInstance());
    vm.translateSourceCode("jmp last\ndb " + std::string(32768 - 6, 'x') + "\nlast: halt");
    EXPECT_NO_THROW(vm.start());
}

TEST(InstructionsTests, ZeroDivisorsLeaveDestinationRegistersUnchanged)
{
    Memory memory;
    Registers registers;
    Instructions instructions(memory, registers, *ofsim_events::EventProcessor::getInstance());
    unsigned char args[] = {0, 1};
    registers[0] = 17;
    registers[1] = 0;
    EXPECT_THROW(instructions.div(args), std::domain_error);
    EXPECT_THROW(instructions.mod(args), std::domain_error);
    EXPECT_EQ(registers[0], 17);

    registers.fl(0, 17.5);
    for (const double zero : {0.0, -0.0})
    {
        registers.fl(1, zero);
        EXPECT_THROW(instructions.fdiv(args), std::domain_error);
        EXPECT_DOUBLE_EQ(registers.fl(0), 17.5);
    }

    registers[1] = 5;
    instructions.div(args);
    EXPECT_EQ(registers[0], 3);
    registers[0] = 17;
    instructions.mod(args);
    EXPECT_EQ(registers[0], 2);
    registers.fl(1, -2.5);
    instructions.fdiv(args);
    EXPECT_DOUBLE_EQ(registers.fl(0), -7.0);
}

TEST(InstructionsTests, ShiftsValidateCountsAndPreserveUnsignedSemantics)
{
    Memory memory;
    Registers registers;
    Instructions instructions(memory, registers, *ofsim_events::EventProcessor::getInstance());
    unsigned char args[] = {0, 1};
    constexpr unsigned int width = std::numeric_limits<unsigned int>::digits;
    for (const unsigned int count : {width, width + 1, std::numeric_limits<unsigned int>::max()})
    {
        registers[0] = 17;
        registers[1] = count;
        EXPECT_THROW(instructions.vshl(args), std::domain_error);
        EXPECT_THROW(instructions.vshr(args), std::domain_error);
        EXPECT_EQ(registers[0], 17);
    }

    registers[0] = 17;
    registers[1] = 0;
    instructions.vshl(args);
    instructions.vshr(args);
    EXPECT_EQ(registers[0], 17);
    registers[0] = 1;
    registers[1] = width - 1;
    instructions.vshl(args);
    EXPECT_EQ(registers[0], 1u << (width - 1));
    instructions.vshr(args);
    EXPECT_EQ(registers[0], 1);
    registers[0] = std::numeric_limits<unsigned int>::max();
    registers[1] = 1;
    instructions.vshl(args);
    EXPECT_EQ(registers[0], std::numeric_limits<unsigned int>::max() - 1);
}
} // namespace vm_tests
