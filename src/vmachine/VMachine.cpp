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
    sourcePath = _sourcePath;
    std::string source_code = translator->translateSourceFile(_sourcePath);
    loadCode();

    return source_code;
}

void ofsim_vm::VMachine::translateSourceCode(std::string sourceCode)
{
    translator->translateSourceString(sourceCode);
    loadCode();
}

void ofsim_vm::VMachine::loadCode()
{
    memory->clear(); // clear old program before load new one
    Memory::memcopy(translator->code, memory->mem, 0, 0, translator->getCodeSize());
}

void VMachine::executionLoop()
{
    // code execution:
    unsigned int opcode = memory->fetchByte(0);
    while (opcode != opcodes->getOpcode("halt") && !shouldStop)
    {
        // decode and execute instruction:
        unsigned int args_size = opcodes->getInstrSize(opcode);
        std::vector<unsigned char> args(args_size);
        Memory::memcopy(memory->mem, args.data(), pc + 1, 0, args_size);
        instructions->call(opcode, args.data());
        pc = registers->pc();
        if (oldpc == pc)
        {
            // there was no jump - update program counter:
            pc += args_size + 1;
            registers->pc(pc);
        }
        oldpc = pc;

        // fetch another opcode
        opcode = memory->fetchByte(pc);
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
    oldpc = 0;
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
