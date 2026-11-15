#pragma once

#include <map>
#include <string>

#include "Opcodes.h"

namespace ofsim_vm
{
class Translator
{
public:
    std::string translateSourceFile(const char* sourcePath);
    void translateSourceString(const std::string& sourceString);
    void translateLine(std::string sourceLine, int lineNumber);

    unsigned int getCodeSize() const
    {
        return instr_addr;
    }

    std::string getTranslationErrors() const
    {
        return translationErrors;
    }
    void clearTranslationErrors()
    {
        translationErrors.clear();
    }

    unsigned char code[64 * 512]{}; // code buffer

private:
    Opcodes opcodes;
    unsigned int instr_addr{0};
    std::string translationErrors;
    std::map<std::string, unsigned int> labelDict;

    void clear();
    void processLine(std::string line, int lineNumber, bool emit, bool defineLabels);
    void checkCapacity(std::size_t length) const;
    unsigned int wordOperand(const std::string& operand, bool resolveLabels) const;
    double doubleOperand(const std::string& operand, bool resolveLabels) const;
};
} // namespace ofsim_vm
