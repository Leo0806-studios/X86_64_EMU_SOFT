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
				
				std::print("value of destination register {:#X} (signed {}) before execution\n", core.GetRegisterValue(instruction.DestinationRegister), static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister)));
				core.SetRegisterValueMasked(instruction.DestinationRegister, val,instruction.DestinationSize);
				std::print("value of destination register {:#X} (signed {}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister), static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister)));
				return;
			}
			if (instruction.ModRM.mod == 0b11) {
				std::print("Executing instruction: MOV r/m{} {}, r{} {}\n", instruction.DestinationSize, VirtualCore::getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize), instruction.SourceSize, VirtualCore::getSubregisterFromSize(instruction.SourceRegister, instruction.SourceSize));
				const uint64_t sourceVal = core.GetRegisterValue(instruction.SourceRegister);
				std::print("value of Source Register {:#X} (signed {}) before execution\n", sourceVal, static_cast<int64_t>(sourceVal));
				core.SetRegisterValueMasked(instruction.DestinationRegister, sourceVal, instruction.DestinationSize);
				std::print("value of destination register {:#X} (signed {}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister), static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister)));
				return;
			}
			switch (instruction.ModRM.mod) {
				case 0b00:
				case 0b01:
				case 0b10:
					std::print("Memory operands are not supported yet for MOV\n");
					throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands are not supported yet for MOV");
				default:
					throw EXCEPTIONS::UNDEFINED_OPCODE("Invalid ModRM mod field for MOV instruction");
			}
	}

}// namespace X86_64_EMU_SOFT::SYSTEM::CPU