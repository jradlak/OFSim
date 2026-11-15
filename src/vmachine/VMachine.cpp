#include "VMachine.h"

#include <stdexcept>
#include <vector>

using namespace ofsim_vm;

VMachine::VMachine(ofsim_events::EventProcessor& _eventProcessor, u64 runId)
    : eventProcessor(_eventProcessor)
{
    memory = std::make_unique<Memory>();
    registers = std::make_unique<Registers>();
    opcodes = std::make_unique<Opcodes>();
    instructions = std::make_unique<Instructions>(*memory, *registers, eventProcessor, runId);
    translator = std::make_unique<Translator>();
}

std::string VMachine::translateSourceCodeFromFile(const char* _sourcePath)
{
    clearMemory();
    sourcePath = _sourcePath;
    std::string source_code = translator->translateSourceFile(_sourcePath);
    loadCode();

    return source_code;
}

void ofsim_vm::VMachine::translateSourceCode(std::string sourceCode)
{
    clearMemory();
    translator->translateSourceString(sourceCode);
    loadCode();
}

void ofsim_vm::VMachine::loadCode()
{
    clearMemory();
    const auto size = translator->getCodeSize();
    if (size == 0)
        throw std::runtime_error("VM program is empty");
    memory->writeBlock(0, std::span<const unsigned char>(translator->code, size));
    loadedCodeSize = size;
}

void VMachine::executionLoop()
{
    while (!shouldStop)
    {
        if (pc >= loadedCodeSize)
            throw std::out_of_range("VM instruction address outside loaded program: " +
                                    std::to_string(pc));
        const auto opcode = memory->fetchByte(pc);
        unsigned int argsSize;
        try
        {
            argsSize = opcodes->getInstrSize(opcode);
        }
        catch (const std::invalid_argument& error)
        {
            throw std::invalid_argument(std::string(error.what()) + " at address " +
                                        std::to_string(pc));
        }
        if (argsSize > loadedCodeSize - pc - 1)
            throw std::out_of_range("Incomplete VM instruction at address " + std::to_string(pc));
        std::vector<unsigned char> args(argsSize);
        memory->readBlock(pc + 1, args);
        for (unsigned int i = 0; i < opcodes->getRegisterOperandCount(opcode); ++i)
        {
            if (args[i] >= number_of_registers)
                throw std::out_of_range(
                    "VM register index out of range: " + std::to_string(args[i]) + " at address " +
                    std::to_string(pc));
        }
        if (opcode == opcodes->getOpcode("halt"))
            return;

        // Ordinary instructions fall through; taken jumps explicitly replace this address.
        registers->pc(pc + argsSize + 1);
        try
        {
            instructions->call(opcode, args.data());
        }
        catch (const std::domain_error& error)
        {
            throw std::domain_error(std::string(error.what()) + " at address " +
                                    std::to_string(pc));
        }
        pc = registers->pc();
    }
}

void VMachine::stop()
{
    shouldStop = true;
    // Memory belongs to the worker until it exits. Collect it before resetting.
}

void VMachine::restart()
{
    // The caller must collect the previous worker before restarting.
    if (started)
        throw std::logic_error("Cannot restart a running VM");
    registers->clear();
    pc = 0;
    loadCode();
    shouldStop = false;
    start();
}

void VMachine::start()
{
    // A stop requested before this thread starts must not be lost.
    started = true;
    std::cout << "VMachine execution loop started!\n";
    try
    {
        executionLoop();
    }
    catch (...)
    {
        started = false;
        throw;
    }
    started = false;
}
