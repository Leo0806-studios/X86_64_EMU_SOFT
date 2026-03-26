#pragma once
#include <unordered_map>
#include <array>
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h" 
#include "SYSTEM/CPU/VCORE.h"
#include "SYSTEM/CPU/INSTRUCTIONS/OPCODE_BYTES.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU {

	class ExecutionEngine {
		using HandlerFunc = void(*)(VirtualCore& core,const  INSTRUCTIONS::Instruction& instruction);
		static const bool HandlerFuncsSetupDone;
	public:
		static std::array<HandlerFunc,256> HandlerFuncs;
	};
}//X86_64_EMU_SOFT::SYSTEM::CPU