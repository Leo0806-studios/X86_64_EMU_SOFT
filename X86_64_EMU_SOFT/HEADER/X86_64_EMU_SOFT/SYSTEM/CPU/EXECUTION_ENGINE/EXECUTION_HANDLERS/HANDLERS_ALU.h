#pragma once
#include <cstdint>
#include <print>
#include <source_location>
#include <string.h>
#include <tracy/Tracy.hpp>
#include <tuple>
#include <HELPERS/MACROS.h>
#include "HELPERS/GLOBALS.h"
#include "SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h"
#include "SYSTEM/CPU/EXECUTION_ENGINE/EXECUTION_ENGINE.h"
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/VCORE.h"

#include "HELPERS/REDEFINE_MACROS.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU {
	inline void Handle_ADD(VirtualCore& core, const INSTRUCTIONS::Instruction& instruction) {
		ZoneNamed(Handle_ADD, true);
		if (instruction.ImmediateSizeBytes > 0) {
			RunIfMinimalOrHigherTraceMode(std::print("Executing instruction: ADD r/m{} {}, imm{} \n", instruction.DestinationSize, VirtualCore::getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize), instruction.SourceSize););
			uint64_t sourceVal = 0;
			std::memcpy(&sourceVal, instruction.ImmediateBytes.data(), instruction.InstructionLengthBytes);
			const uint64_t destVal = core.GetRegisterValue(instruction.DestinationRegister);
			RunIfFullTraceMode(std::print("value of Destintion register: {:#X} (signed {}), value of immediat {:#X} (signed: {} before execution\n", destVal, static_cast<int64_t>(destVal), sourceVal, static_cast<int64_t>(sourceVal)););
				core.SetRegisterValueMasked(instruction.DestinationRegister, destVal + sourceVal, instruction.DestinationSize);
			RunIfFullTraceMode(std::print("value of Destiontion Register: {:#X} (signed {}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister), static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister))););
				return;
		}
		if (instruction.ModRM.mod == 0b11) {

			RunIfMinimalOrHigherTraceMode(std::print("Executing instruction: ADD r/m{} {}, r{} {}\n", instruction.DestinationSize, VirtualCore::getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize), instruction.SourceSize, VirtualCore::getSubregisterFromSize(instruction.SourceRegister, instruction.SourceSize)););
				const uint64_t sourceVal = core.GetRegisterValue(instruction.SourceRegister);
			const uint64_t destVal = core.GetRegisterValue(instruction.DestinationRegister);
			RunIfFullTraceMode(std::print("value of Destintion register: {:#X} (signed: {}), value of Source Register {:#X} (signed: {}) before execution\n", destVal, static_cast<int64_t>(destVal), sourceVal, static_cast<int64_t>(sourceVal)););
			core.SetRegisterValueMasked(instruction.DestinationRegister, destVal + sourceVal, instruction.DestinationSize);
			RunIfFullTraceMode(std::print("value of Destiontion Register: {:#X} (signed: {}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister), static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister))););
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
	inline void Handle_SUB(VirtualCore& core, const  INSTRUCTIONS::Instruction& instruction) {
		ZoneNamed(HandleSub, true);
		if (instruction.ImmediateSizeBytes > 0) {
			RunIfMinimalOrHigherTraceMode(std::print("Executing instruction: SUB r/m{} {}, imm{} \n", instruction.DestinationSize, VirtualCore::getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize), instruction.SourceSize););
			uint64_t sourceVal = 0;
			std::memcpy(&sourceVal, instruction.ImmediateBytes.data(), instruction.InstructionLengthBytes);
			const uint64_t destVal = core.GetRegisterValue(instruction.DestinationRegister);
			RunIfFullTraceMode(std::print("value of Destintion register: {:#X} (signed {}), value of immediat {:#X} (signed {}) before execution\n", destVal, static_cast<int64_t>(destVal), sourceVal, static_cast<int64_t>(sourceVal)););
			core.SetRegisterValueMasked(instruction.DestinationRegister, destVal - sourceVal, instruction.DestinationSize);
			RunIfFullTraceMode(std::print("value of Destiontion Register: {:#X} (signed {}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister), static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister))););
			return;
		}
		if (instruction.ModRM.mod == 0b11) {
			RunIfMinimalOrHigherTraceMode(std::print("Executing instruction: SUB r/m{} {}, r{} {}\n", instruction.DestinationSize, VirtualCore::getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize), instruction.SourceSize, VirtualCore::getSubregisterFromSize(instruction.SourceRegister, instruction.SourceSize)););
			const uint64_t sourceVal = core.GetRegisterValue(instruction.SourceRegister);
			const uint64_t destVal = core.GetRegisterValue(instruction.DestinationRegister);
			RunIfFullTraceMode(std::print("value of Destintion register: {:#X} (signed {}), value of Source Register {:#X} (signed: {}) before execution\n", destVal, static_cast<int64_t>(destVal), sourceVal, static_cast<int64_t>(sourceVal)););
			core.SetRegisterValueMasked(instruction.DestinationRegister, destVal - sourceVal, instruction.DestinationSize);
			RunIfFullTraceMode(std::print("value of Destiontion Register: {:#X} (signed {}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister), static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister))););
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

	inline void Handle_OR(VirtualCore& core, const  INSTRUCTIONS::Instruction& instruction) {
		ZoneNamed(Handle_OR, true);
		std::ignore = core;
		std::ignore = instruction;
		if (instruction.ImmediateSizeBytes > 0) {
			RunIfMinimalOrHigherTraceMode(std::print("Executing instruction: OR r/m{} {}, imm{} \n", instruction.DestinationSize, VirtualCore::getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize), instruction.SourceSize););
			uint64_t sourceVal = 0;
			std::memcpy(&sourceVal, instruction.ImmediateBytes.data(), instruction.InstructionLengthBytes);
			const uint64_t destVal = core.GetRegisterValue(instruction.DestinationRegister);
			RunIfFullTraceMode(std::print("value of Destintion register: {:#X} (binary {:#B}), value of immediat {:#X} (binary {:#B}) before execution\n", destVal, destVal, sourceVal, sourceVal););
			core.SetRegisterValueMasked(instruction.DestinationRegister, destVal | sourceVal, instruction.DestinationSize);
			RunIfFullTraceMode(std::print("value of Destiontion Register: {:#X} (binary {:#B}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister), core.GetRegisterValue(instruction.DestinationRegister)););
			return;
		}
		if (instruction.ModRM.mod == 0b11) {
			RunIfMinimalOrHigherTraceMode(std::print("Executing instruction: OR r/m{} {}, r{} {}\n", instruction.DestinationSize, VirtualCore::getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize), instruction.SourceSize, VirtualCore::getSubregisterFromSize(instruction.SourceRegister, instruction.SourceSize)););
			const uint64_t sourceVal = core.GetRegisterValue(instruction.SourceRegister);
			const uint64_t destVal = core.GetRegisterValue(instruction.DestinationRegister);
			RunIfFullTraceMode(std::print("value of Destintion register: {:#X} (binary {:#B}), value of Source Register {:#X} (binary: {:#B}) before execution\n", destVal, destVal, sourceVal, sourceVal););
			core.SetRegisterValueMasked(instruction.DestinationRegister, destVal | sourceVal, instruction.DestinationSize);
			RunIfFullTraceMode(std::print("value of Destiontion Register: {:#X} (binary {:#B}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister), core.GetRegisterValue(instruction.DestinationRegister)););
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
}// namespace X86_64_EMU_SOFT::SYSTEM::CPU