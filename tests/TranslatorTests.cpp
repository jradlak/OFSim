#include <gtest/gtest.h>
#include <string>

#include "../src/vmachine/Translator.h"

namespace vm_tests
{
	using namespace ofsim_vm;

	std::string hello_world = R"(
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

	TEST(TranslatorTest, translateSimpleLine) 
	{
		Translator translator;
		std::string codeLine = "set r1, 65432";
		translator.translateLine(codeLine, 1);
		EXPECT_EQ(translator.code[0], 0x03);
		EXPECT_EQ(translator.code[1], 0x01);
		EXPECT_EQ(translator.code[2], 152);
		EXPECT_EQ(translator.code[3], 255);
		EXPECT_EQ(translator.code[4], 0);
	}

	TEST(TranslatorTest, translateTheWholeSourceCode) 
	{		
		Translator translator;
		translator.translateSourceString(hello_world);
		EXPECT_EQ(translator.code[0], 0x03);
		EXPECT_EQ(translator.code[57], 0x64);
		EXPECT_EQ(translator.code[58], 0);
	}
}