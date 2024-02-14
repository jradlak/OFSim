#include <gtest/gtest.h>

#include "../src/infrastructure/CommunicationBus.h"
#include "../src/vmachine/VMachine.h"

namespace vm_tests
{
	using namespace ofsim_vm;

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
		com_bus::Tbus_data commandBus;
		VMachine vm(&commandBus);
		vm.translateSourceCode(hello_world_vm);
		
		// Act
		vm.start();
		
		// Assert
		EXPECT_EQ(vm.getMemory()->fetchByte(255), 0);
		EXPECT_EQ(vm.getMemory()->fetchByte(256), 72);
		EXPECT_EQ(vm.getMemory()->fetchByte(257), 101);
	}		
}