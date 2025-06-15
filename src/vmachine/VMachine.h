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

#include "../math/MathTypes.h"
#include "../infrastructure/EventProcessor.h"

namespace ofsim_vm
{
	class VMachine
	{
	public:
		VMachine(ofsim_events::EventProcessor& _event_processor);

		// Translates source code from the file path, and returs loaded source txt file
		std::string translateSourceCodeFromFile(std::string _sourcePath);

		void translate_source_code(std::string sourceCode);

		void load_code();

		void start();

		void stop();

		void restart();

		void clear_memory() { memory.clear(); }

		void provide_source_path(const char *_source_path) { source_path = _source_path; }

		void take_a_nap() { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }

		Memory &getMemory() { return memory; }

		bool isStarted() { return started; }		

	private:
		void executionLoop();

		ofsim_events::EventProcessor& event_processor;

		Memory memory;
		Registers registers;
		Opcodes opcodes;		
		Instructions instructions{memory, registers, event_processor};
		Translator translator;

		std::string source_path{""};

		u32 pc{0};
		u32 oldpc{0};

		bool should_stop{true};
		bool started{false};
	};
} // namespace ofsim_vm