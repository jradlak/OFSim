#include "Translator.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iterator>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "Registers.h"

using namespace ofsim_vm;

namespace
{
std::string trim(const std::string& text)
{
    const auto first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return {};

    return text.substr(first, text.find_last_not_of(" \t\r\n") - first + 1);
}

bool isLabel(const std::string& text)
{
    const auto firstCharacter = [](unsigned char ch)
    { return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || ch == '.'; };

    if (text.empty() || !firstCharacter(text.front()))
        return false;

    return std::all_of(text.begin(), text.end(), [&](unsigned char ch)
                       { return firstCharacter(ch) || (ch >= '0' && ch <= '9'); });
}

unsigned char registerOperand(const std::string& operand)
{
    if (operand.size() < 2 || (operand[0] != 'r' && operand[0] != 'f'))
        throw std::invalid_argument("Invalid register: " + operand);

    unsigned int index = 0;
    const auto result = std::from_chars(operand.data() + 1, operand.data() + operand.size(), index);
    if (result.ec != std::errc{} || result.ptr != operand.data() + operand.size() ||
        index >= number_of_registers)
        throw std::out_of_range("VM register index out of range: " + operand);

    return static_cast<unsigned char>(index);
}

std::vector<std::string> splitOperands(const std::string& text)
{
    std::vector<std::string> operands;
    if (text.empty())
        return operands;

    std::size_t first = 0;
    while (true)
    {
        const auto comma = text.find(',', first);
        const auto operand = trim(text.substr(first, comma - first));
        if (operand.empty())
            throw std::invalid_argument("Missing operand");

        operands.push_back(operand);
        if (comma == std::string::npos)
            return operands;

        first = comma + 1;
    }
}
} // namespace

void Translator::clear()
{
    instr_addr = 0;
    labelDict.clear();
    translationErrors.clear();
    std::fill(std::begin(code), std::end(code), 0);
}

std::string Translator::translateSourceFile(const char* sourcePath)
{
    clear();

    std::ifstream file(sourcePath, std::ios::binary);
    if (!file)
    {
        translationErrors = "Unable to open VM source file: " + std::string(sourcePath);
        throw std::runtime_error(translationErrors);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    if (file.bad())
    {
        translationErrors = "Unable to read VM source file: " + std::string(sourcePath);
        throw std::runtime_error(translationErrors);
    }

    const auto source = buffer.str();
    translateSourceString(source);

    return source;
}

void Translator::translateSourceString(const std::string& sourceString)
{
    clear();

    try
    {
        for (const bool emit : {false, true})
        {
            instr_addr = 0;
            std::istringstream source(sourceString);
            std::string line;
            int lineNumber = 1;

            while (std::getline(source, line))
                processLine(line, lineNumber++, emit, !emit);
        }
    }
    catch (const std::exception& error)
    {
        const std::string message = error.what();
        clear();
        translationErrors = message;
        throw;
    }
}

void Translator::translateLine(std::string sourceLine, int lineNumber)
{
    try
    {
        processLine(std::move(sourceLine), lineNumber, true, true);
    }
    catch (const std::exception& error)
    {
        const std::string message = error.what();
        clear();
        translationErrors = message;
        throw;
    }
}

void Translator::checkCapacity(std::size_t length) const
{
    if (length > sizeof(code) - instr_addr)
        throw std::out_of_range("VM program exceeds the 32768-byte code buffer");
}

unsigned int Translator::wordOperand(const std::string& operand, bool resolveLabels) const
{
    if (isLabel(operand))
    {
        if (!resolveLabels)
            return 0;

        const auto label = labelDict.find(operand);
        if (label == labelDict.end())
            throw std::invalid_argument("Unknown label: " + operand);

        return label->second;
    }

    std::size_t consumed = 0;
    long long value;

    try
    {
        value = std::stoll(operand, &consumed, 10);
    }
    catch (const std::exception&)
    {
        throw std::invalid_argument("Invalid 32-bit integer: " + operand);
    }

    if (consumed != operand.size() || value < std::numeric_limits<std::int32_t>::min() ||
        value > std::numeric_limits<std::uint32_t>::max())
        throw std::out_of_range("Invalid 32-bit integer: " + operand);

    return static_cast<unsigned int>(value);
}

double Translator::doubleOperand(const std::string& operand, bool resolveLabels) const
{
    const bool decimalFraction =
        operand.size() > 1 && operand[0] == '.' && operand[1] >= '0' && operand[1] <= '9';
    if (isLabel(operand) && !decimalFraction)
        return wordOperand(operand, resolveLabels);

    std::size_t consumed = 0;
    double value;

    try
    {
        value = std::stod(operand, &consumed);
    }
    catch (const std::exception&)
    {
        throw std::invalid_argument("Invalid finite floating-point value: " + operand);
    }

    if (consumed != operand.size() || !std::isfinite(value))
        throw std::invalid_argument("Invalid finite floating-point value: " + operand);

    return value;
}

void Translator::processLine(std::string line, int lineNumber, bool emit, bool defineLabels)
{
    try
    {
        line = trim(line);
        if (line.empty() || line.front() == ';')
            return;

        const auto colon = line.find(':');
        const auto separator = line.find_first_of(" \t");
        if (colon != std::string::npos && colon < separator && colon < line.find(';'))
        {
            const auto label = line.substr(0, colon);
            if (!isLabel(label))
                throw std::invalid_argument("Invalid label: " + label);
            if (defineLabels && !labelDict.emplace(label, instr_addr).second)
                throw std::invalid_argument("Duplicate label: " + label);

            line = trim(line.substr(colon + 1));
        }

        if (line.empty() || line.front() == ';')
            return;

        const auto tokenEnd = line.find_first_of(" \t;");
        const auto token = line.substr(0, tokenEnd);

        // db stores literal text, including semicolons; instruction comments start at ';'.
        if (token == "db")
        {
            const auto data = tokenEnd == std::string::npos || line[tokenEnd] == ';'
                                  ? std::string{}
                                  : line.substr(tokenEnd + 1);
            if (data.empty())
                throw std::invalid_argument("Missing db data");

            checkCapacity(data.size());

            if (emit)
                std::memcpy(code + instr_addr, data.data(), data.size());
            instr_addr += static_cast<unsigned int>(data.size());
            return;
        }

        line = trim(line.substr(0, line.find(';')));
        const auto operandStart = line.find_first_of(" \t");
        const auto operands = splitOperands(
            operandStart == std::string::npos ? std::string{} : trim(line.substr(operandStart)));

        const auto opcode = opcodes.getOpcode(token);
        if (opcode == 0x100)
            throw std::invalid_argument("Unknown instruction: " + token);

        const auto size = opcodes.getInstrSize(opcode);
        const auto registers = opcodes.getRegisterOperandCount(opcode);
        const bool hasWord = opcode == 0x3 || (opcode >= 0x1C && opcode <= 0x22);
        const bool hasDouble = opcode == 0x4;
        if (operands.size() != registers + hasWord + hasDouble)
            throw std::invalid_argument("Incorrect operand count for " + token);

        checkCapacity(size + 1);

        std::array<unsigned char, 10> bytes{};
        bytes[0] = static_cast<unsigned char>(opcode);
        for (unsigned int i = 0; i < registers; ++i)
            bytes[i + 1] = registerOperand(operands[i]);

        if (hasWord)
        {
            const auto value = wordOperand(operands[registers], emit);
            std::memcpy(bytes.data() + registers + 1, &value, sizeof(value));
        }

        if (hasDouble)
        {
            const auto value = doubleOperand(operands[registers], emit);
            std::memcpy(bytes.data() + registers + 1, &value, sizeof(value));
        }

        if (emit)
            std::memcpy(code + instr_addr, bytes.data(), size + 1);
        instr_addr += size + 1;
    }
    catch (const std::exception& error)
    {
        throw std::runtime_error("VM translation error at line " + std::to_string(lineNumber) +
                                 ": " + error.what());
    }
}
