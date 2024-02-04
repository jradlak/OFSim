#include <gtest/gtest.h>

#include "../src/vmachine/Translator.h"

namespace vm_tests
{
	using namespace ofsim_vm;

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
		translator.translateSourceFile("../../../tests/hello_world.oasm");
		EXPECT_EQ(translator.code[0], 0x03);
		EXPECT_EQ(translator.code[57], 0x64);
		EXPECT_EQ(translator.code[58], 0);
	}
}