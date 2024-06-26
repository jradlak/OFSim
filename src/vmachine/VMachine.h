#pragma once

#include <chrono>
#include <memory>
#include <thread>
#include <string>

#include "Memory.h"
#include "Registers.h"
#include "Instructions.h"
#include "Translator.h"
#include "Opcodes.h"

#include "../math_and_physics/MathTypes.h"
#include "../infrastructure/EventProcessor.h"

namespace ofsim_vm
{
	class VMachine
	{
	public:
		VMachine(ofsim_events::EventProcessor& _eventProcessor);

		// Translates source code from the file path, and returs loaded source txt file
		std::string translateSourceCodeFromFile(const char* _sourcePath);

		void translateSourceCode(std::string sourceCode);

		void loadCode();

		void start();

		void stop();

		void restart();

		void clearMemory() { memory->clear(); }

		void provideSourcePath(const char *_sourcePath) { sourcePath = _sourcePath; }

		void takeANap() { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }

		Memory *getMemory() { return memory.get(); }

		bool isStarted() { return started; }		

	private:
		void executionLoop();

		ofsim_events::EventProcessor& eventProcessor;

		std::unique_ptr<Memory> memory;
		std::unique_ptr<Registers> registers;
		std::unique_ptr<Opcodes> opcodes;
		std::unique_ptr<Instructions> instructions;
		std::unique_ptr<Translator> translator;

		const char *sourcePath{nullptr};

		u32 pc{0};
		u32 oldpc{0};

		bool shouldStop{true};
		bool started{false};
	};
} // namespace ofsim_vm