#pragma once
#include "SYSTEM/CPU/VCORE.h"
#include <cstdint>
#include <string.h>
#include <tuple>
#include <print>
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU {
	inline void Handle_UD(VirtualCore& core,const INSTRUCTIONS::Instruction& instruction) {
		std::ignore = core;
		std::ignore = instruction;
		throw EXCEPTIONS::UNDEFINED_OPCODE("UD instruction executed");
	}
	inline void Handle_NOP(VirtualCore& core,const INSTRUCTIONS::Instruction& instruction) {
		std::ignore = core;
		std::ignore = instruction;
		std::print("Executing instruction: NOP\n");
	}
	inline void Handle_MOV(VirtualCore& core,const INSTRUCTIONS::Instruction& instruction) {
		
			if (instruction.ImmediateSizeBytes > 0) {
				std::print("Executing instruction: MOV r/m{} {}, imm{}\n", instruction.DestinationSize, VirtualCore::getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize), instruction.SourceSize);
				uint64_t val = 0;

				__assume(instruction.ImmediateBytes.data());
				std::memcpy(&val, instruction.ImmediateBytes.data(), sizeof(uint64_t));
				uint64_t tmpval = core.GetRegisterValue(instruction.DestinationRegister);
				__assume(instruction.DestinationSize < 64);
				const uint64_t mask = (1ULL << instruction.DestinationSize) - 1;
				tmpval &= ~mask;
				tmpval |= val;
				std::print("value of destination register {:#X} (signed {}) before execution\n", core.GetRegisterValue(instruction.DestinationRegister), static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister)));
				core.SetRegisterValue(instruction.DestinationRegister, tmpval);
				std::print("value of destination register {:#X} (signed {}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister), static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister)));
			}
			else {
				throw EXCEPTIONS::UNDEFINED_OPCODE("Register to memory or register to register is not yet supported for MOV");
			}
	}

}// namespace X86_64_EMU_SOFT::SYSTEM::CPU