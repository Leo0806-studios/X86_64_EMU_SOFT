#include <string.h>
#include <format>
#include <cstdint>
#include <print>
#include <unordered_map>
#include <exception>
#include <array>
#include <utility>
#include <tuple>
#include <tracy/Tracy.hpp>
#include "SYSTEM/CPU/EXECUTION_ENGINE/EXECUTION_ENGINE.h"
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/INSTRUCTIONS/OPCODE_BYTES.h"
#include "SYSTEM/CPU/VCORE.h"
#include "SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h"
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_ENGINE.h"
#include "SYSTEM/CPU/EXECUTION_ENGINE/EXECUTION_HANDLERS/HANDLERS_ALU.h"
#include "SYSTEM/CPU/EXECUTION_ENGINE/EXECUTION_HANDLERS/HANDLER_SPECIAL.h"
#include <HELPERS/MACROS.h>
namespace X86_64_EMU_SOFT::SYSTEM::CPU{
	namespace {
		[[noreturn]] void UndefinedOpcodeHandler(VirtualCore& core,const INSTRUCTIONS::Instruction& instruction) {
			DeepZoneScoped;
			std::ignore = core;
			std::ignore = instruction;
			throw EXCEPTIONS::UNDEFINED_OPCODE("undefined opcode Handler called\n the insturction that was about to be executed was decoded but had no apropriate execution handler installed");
		}
	}
	std::array<ExecutionEngine::HandlerFunc,256> ExecutionEngine::HandlerFuncs;
	const bool ExecutionEngine::HandlerFuncsSetupDone = []() {
		try {
			for (auto& handler : HandlerFuncs) {
				handler = UndefinedOpcodeHandler;
			}
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::InstructionType::ADD)] = Handle_ADD;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::InstructionType::SUB)] = Handle_SUB;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::InstructionType::OR)] = Handle_OR;



			HandlerFuncs[std::to_underlying(INSTRUCTIONS::InstructionType::MOV)] = Handle_MOV;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::InstructionType::NOP)] = Handle_NOP;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::InstructionType::UD)] = Handle_UD;
		}
		catch (...) {
			std::print("Error setting up handler functions for execution engine\n");
			std::terminate();
		}

		return true;
		}();
	// void ExecutionEngine::ExecuteInstruction(VirtualCore& core, const INSTRUCTIONS::Instruction& instruction)
	//{
	//	 ZoneScoped;
	//	 ExecutionEngine::HandlerFuncs[std::to_underlying(instruction.Type)](core, instruction);
	//}
}