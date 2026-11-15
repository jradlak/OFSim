#include <gtest/gtest.h>
#include <string>
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <limits>
#include <stdexcept>

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
} // namespace vm_tests
namespace vm_tests
{
TEST(TranslatorTest, CodeBufferAcceptsExactFitAndRejectsOverflowWithoutPartialOutput)
{
    Translator translator;
    const auto capacity = sizeof(translator.code);
    translator.translateSourceString("db " + std::string(capacity - 1, 'x') + "\nhalt\n");
    EXPECT_EQ(translator.getCodeSize(), capacity);
    EXPECT_EQ(translator.code[capacity - 1], 0x24);
    EXPECT_THROW(translator.translateSourceString("db " + std::string(capacity, 'x') + "\nhalt\n"),
                 std::runtime_error);
    EXPECT_EQ(translator.getCodeSize(), 0);
    EXPECT_TRUE(std::all_of(std::begin(translator.code), std::end(translator.code),
                            [](auto byte) { return byte == 0; }));
    EXPECT_NE(translator.getTranslationErrors().find("line 2"), std::string::npos);
    EXPECT_THROW(translator.translateSourceString("db " + std::string(capacity + 1, 'x')),
                 std::runtime_error);
    EXPECT_EQ(translator.getCodeSize(), 0);
    EXPECT_THROW(
        translator.translateSourceString("db " + std::string(capacity - 5, 'x') + "\nset r0, 1"),
        std::runtime_error);
}

TEST(TranslatorTest, RepeatedTranslationsClearCodeLabelsAndErrors)
{
    Translator translator;
    translator.translateSourceString("old: set r0, 42\nhalt\n");
    EXPECT_THROW(translator.translateSourceString("jmp old\n"), std::runtime_error);
    EXPECT_EQ(translator.getCodeSize(), 0);
    EXPECT_NE(translator.getTranslationErrors().find("Unknown label: old"), std::string::npos);
    translator.translateSourceString("halt\n");
    EXPECT_EQ(translator.getCodeSize(), 1);
    EXPECT_TRUE(translator.getTranslationErrors().empty());
    EXPECT_EQ(translator.code[1], 0);
    EXPECT_THROW(translator.translateSourceString("halt\nunknown\n"), std::runtime_error);
    EXPECT_EQ(translator.getCodeSize(), 0);
    EXPECT_EQ(translator.code[0], 0);
    translator.translateSourceString("new: halt\n");
    EXPECT_EQ(translator.getCodeSize(), 1);
}

TEST(TranslatorTest, LabelsAfterDataAndInlineCommentsHaveCorrectAddresses)
{
    Translator translator;
    translator.translateSourceString("\t jmp after ; forward jump\r\n"
                                     "db abc\r\nafter: halt ; end\r\n");
    unsigned int target = 0;
    std::memcpy(&target, translator.code + 1, sizeof(target));
    EXPECT_EQ(target, 8);
    EXPECT_EQ(translator.getCodeSize(), 9);
    EXPECT_EQ(translator.code[8], 0x24);
    translator.translateSourceString("set r0, data\nhalt\ndata: db  hi;there\n");
    EXPECT_EQ(std::string(reinterpret_cast<char*>(translator.code + 7), 9), " hi;there");
    translator.translateSourceString("halt;comment: ignored");
    EXPECT_EQ(translator.getCodeSize(), 1);
}

TEST(TranslatorTest, InvalidSyntaxAndConversionsReportTheirSourceLine)
{
    for (const std::string line : {"unknown",
                                   "set",
                                   "set r0",
                                   "set r0,",
                                   "set r0, 1, 2",
                                   "mov r0 r1",
                                   "halt r0",
                                   "ftc r0",
                                   "jmp missing",
                                   "set r0, missing",
                                   "set r16, 1",
                                   "set r256, 1",
                                   "set r-1, 1",
                                   "set x0, 1",
                                   "mov r0, r1junk",
                                   "fset f16, 1",
                                   "set r0, 4294967296",
                                   "set r0, -2147483649",
                                   "set r0, 12junk",
                                   "fset f0, 1.2junk",
                                   "fset f0, 1e9999",
                                   "fset f0, nan",
                                   "fset f0, inf",
                                   "fset f0, -inf",
                                   "set r0, +",
                                   "db; comment",
                                   "db",
                                   "label: halt\nlabel: halt"})
    {
        SCOPED_TRACE(line);
        Translator translator;
        try
        {
            translator.translateSourceString("; header\n" + line);
            FAIL() << "Invalid source was accepted";
        }
        catch (const std::runtime_error& error)
        {
            EXPECT_NE(std::string(error.what()).find("line "), std::string::npos);
        }
        EXPECT_EQ(translator.getCodeSize(), 0);
        EXPECT_FALSE(translator.getTranslationErrors().empty());
    }
}

TEST(TranslatorTest, NumericLimitsKeepTheNativeInstructionLayout)
{
    Translator translator;
    translator.translateSourceString("set r15, 4294967295\nset r0, -2147483648\n"
                                     "fset f15, -1.25\nfset f0, .5\nhalt");
    unsigned int maximum = 0;
    int minimum = 0;
    double floating = 0;
    std::memcpy(&maximum, translator.code + 2, sizeof(maximum));
    std::memcpy(&minimum, translator.code + 8, sizeof(minimum));
    std::memcpy(&floating, translator.code + 14, sizeof(floating));
    EXPECT_EQ(maximum, std::numeric_limits<unsigned int>::max());
    EXPECT_EQ(minimum, std::numeric_limits<int>::min());
    EXPECT_DOUBLE_EQ(floating, -1.25);
    std::memcpy(&floating, translator.code + 24, sizeof(floating));
    EXPECT_DOUBLE_EQ(floating, 0.5);
}

TEST(TranslatorTest, MissingFileDiscardsPreviouslyTranslatedCode)
{
    Translator translator;
    translator.translateSourceString("halt");
    const auto missing =
        std::filesystem::path(__FILE__).parent_path() / "missing-ofsim-source.oasm";
    ASSERT_FALSE(std::filesystem::exists(missing));
    EXPECT_THROW(translator.translateSourceFile(missing.string().c_str()), std::runtime_error);
    EXPECT_EQ(translator.getCodeSize(), 0);
    EXPECT_FALSE(translator.getTranslationErrors().empty());
}

TEST(TranslatorTest, AllBundledProgramsTranslateWithinTheCodeBuffer)
{
    const auto root = std::filesystem::path(__FILE__).parent_path().parent_path();
    Translator translator;
    for (const auto& entry : std::filesystem::directory_iterator(root / "src/orb_progs"))
    {
        if (entry.path().extension() != ".oasm")
            continue;
        SCOPED_TRACE(entry.path().string());
        EXPECT_NO_THROW(translator.translateSourceFile(entry.path().string().c_str()));
        EXPECT_GT(translator.getCodeSize(), 0);
        EXPECT_LE(translator.getCodeSize(), sizeof(translator.code));
    }
}
} // namespace vm_tests
