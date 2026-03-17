#pragma once
#include <unordered_map>
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h" 
#include "SYSTEM/CPU/VCORE.h"
#include "SYSTEM/CPU/INSTRUCTIONS/OPCODE_BYTES.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU {

	class ExecutionEngine {
		using HandlerFunc = void(*)(VirtualCore& core, INSTRUCTIONS::Instruction& instruction);
		static std::unordered_map<INSTRUCTIONS::InstructionType,HandlerFunc> HandlerFuncs;
		static bool HandlerFuncsSetupDone;
	public:
		static  void ExecuteInstruction(VirtualCore& core, const INSTRUCTIONS::Instruction& instruction);
	};
}//X86_64_EMU_SOFT::SYSTEM::CPU