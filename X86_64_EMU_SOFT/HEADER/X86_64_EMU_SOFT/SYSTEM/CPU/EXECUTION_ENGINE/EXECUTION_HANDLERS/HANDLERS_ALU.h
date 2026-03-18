#pragma once
#include "SYSTEM/CPU/VCORE.h"
#include <cstdint>
#include <string.h>
#include <print>
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU {
	inline void Handle_ADD(VirtualCore& core,const INSTRUCTIONS::Instruction& instruction) {

		if (instruction.ImmediateSizeBytes > 0) {
			std::print("Executing instruction: ADD r/m{} {}, imm{} \n", instruction.DestinationSize, VirtualCore::getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize), instruction.SourceSize);

			uint64_t sourceVal = 0;
			std::memcpy(&sourceVal, instruction.ImmediateBytes.data(), instruction.InstructionLengthBytes);
			const uint64_t destVal = core.GetRegisterValue(instruction.DestinationRegister);
			std::print("value of Destintion register: {:#X} (signed {}), value of immediat {:#X} (signed: {} before execution\n", destVal, static_cast<int64_t>(destVal), sourceVal, static_cast<int64_t>(sourceVal));
			core.SetRegisterValue(instruction.DestinationRegister, destVal + sourceVal);
			std::print("value of Destiontion Register: {:#X} (signed {}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister), static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister)));
			return;
		}
		if (instruction.ModRM.mod == 0b11) {
			std::print("Executing instruction: ADD r/m{} {}, r{} {}\n", instruction.DestinationSize, VirtualCore::getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize), instruction.SourceSize, VirtualCore::getSubregisterFromSize(instruction.SourceRegister, instruction.SourceSize));
			const uint64_t sourceVal = core.GetRegisterValue(instruction.SourceRegister);
			const uint64_t destVal = core.GetRegisterValue(instruction.DestinationRegister);
			std::print("value of Destintion register: {:#X} (signed: {}), value of Source Register {:#X} (signed: {}) before execution\n", destVal, static_cast<int64_t>(destVal), sourceVal, static_cast<int64_t>(sourceVal));
			core.SetRegisterValue(instruction.DestinationRegister, destVal + sourceVal);
			std::print("value of Destiontion Register: {:#X} (signed: {}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister), static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister)));
			return;
		}
		switch (instruction.ModRM.mod) {
			case 0b00:
			case 0b01:
			case 0b10:
				std::print("Memory operands are not supported yet for ADD\n");
				throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands are not supported yet for ADD");
			default:
				throw EXCEPTIONS::UNDEFINED_OPCODE("Invalid ModRM mod field for ADD instruction");
		}



	}
	inline void Handle_SUB(VirtualCore& core,const  INSTRUCTIONS::Instruction& instruction) {

		if (instruction.ImmediateSizeBytes > 0) {
			std::print("Executing instruction: SUB r/m{} {}, imm{} \n", instruction.DestinationSize, VirtualCore::getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize), instruction.SourceSize);
			uint64_t sourceVal = 0;
			std::memcpy(&sourceVal, instruction.ImmediateBytes.data(), instruction.InstructionLengthBytes);
			const uint64_t destVal = core.GetRegisterValue(instruction.DestinationRegister);
			std::print("value of Destintion register: {:#X} (signed {}), value of immediat {:#X} (signed {}) before execution\n", destVal, static_cast<int64_t>(destVal), sourceVal, static_cast<int64_t>(sourceVal));
			core.SetRegisterValue(instruction.DestinationRegister, destVal - sourceVal);
			std::print("value of Destiontion Register: {:#X} (signed {}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister), static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister)));
			return;
		}
		if (instruction.ModRM.mod == 0b11) {
			std::print("Executing instruction: SUB r/m{} {}, r{} {}\n", instruction.DestinationSize, VirtualCore::getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize), instruction.SourceSize, VirtualCore::getSubregisterFromSize(instruction.SourceRegister, instruction.SourceSize));
			const uint64_t sourceVal = core.GetRegisterValue(instruction.SourceRegister);
			const uint64_t destVal = core.GetRegisterValue(instruction.DestinationRegister);
			std::print("value of Destintion register: {:#X} (signed {}), value of Source Register {:#X} (signed: {}) before execution\n", destVal, static_cast<int64_t>(destVal), sourceVal, static_cast<int64_t>(sourceVal));
			core.SetRegisterValue(instruction.DestinationRegister, destVal - sourceVal);
			std::print("value of Destiontion Register: {:#X} (signed {}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister), static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister)));
			return;
		}
		switch (instruction.ModRM.mod) {
			case 0b00:
			case 0b01:
			case 0b10:
				std::print("Memory operands are not supported yet for SUB\n");
				throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands are not supported yet for SUB");
			default:
				throw EXCEPTIONS::UNDEFINED_OPCODE("Invalid ModRM mod field for SUB instruction");
		}

	}

	inline void Handle_OR(VirtualCore& core,const  INSTRUCTIONS::Instruction& instruction) {
		std::ignore = core;
		std::ignore = instruction;
		throw EXCEPTIONS::UNDEFINED_OPCODE("OR instruction is not implemented yet");
	}
}// namespace X86_64_EMU_SOFT::SYSTEM::CPU