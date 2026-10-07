#pragma once
#include <thread>
namespace X86_64_EMU_SOFT::DEBUGGER {
	class Debugger {
		static inline Debugger* instance = nullptr;
		std::jthread debuggerThread;
		
	public:
		static void CreateDebugger();
	};
}