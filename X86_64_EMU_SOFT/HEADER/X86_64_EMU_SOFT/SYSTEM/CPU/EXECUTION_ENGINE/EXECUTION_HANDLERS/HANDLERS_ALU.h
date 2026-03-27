#pragma once
#include <cstdint>
#include <cstring>
#include <HELPERS/MACROS.h>
#include <print>
#include <source_location>
#include <tracy/Tracy.hpp>
#include <tuple>
#include "HELPERS/GLOBALS.h"
#include "SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h"
#include "SYSTEM/CPU/EXECUTION_ENGINE/EXECUTION_ENGINE.h"
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/VCORE.h"

namespace X86_64_EMU_SOFT::SYSTEM::CPU {
	inline void Handle_ADD(VirtualCore& core, const INSTRUCTIONS::Instruction& instruction) {
		ZoneScoped;//NOLINT
		if (instruction.Operand0.Type == INSTRUCTIONS::OPERANDS::OperandType::Memory || instruction.Operand2.Type == INSTRUCTIONS::OPERANDS::OperandType::Memory) {
			throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands are not supported yet for ADD");
		}

		uint64_t sourceVal = 0;
		if (instruction.Operand1.Type == INSTRUCTIONS::OPERANDS::OperandType::Register) {
			const auto& sourceOperand = std::get<INSTRUCTIONS::OPERANDS::RegisterOperand>(instruction.Operand0.Data);
			assert(sourceOperand.Flags & std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister));//the generic add handler only works for gprs
			const REGISTERS::GPR& sourceRegister = static_cast<REGISTERS::GPR&>(*std::bit_cast<REGISTERS::Register*>(sourceOperand.RegisterPointer));
			sourceVal = [&]() -> uint64_t {
				switch (sourceOperand.SizeBits) {
					case 8: {
						if (sourceOperand.Flags & std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister)) {
							return sourceRegister.GetHigh8Bits();
						}
						return sourceRegister.GetLow8Bits();
					}
					case 16:return sourceRegister.GetLow16Bits();
					case 32:return sourceRegister.GetLow32Bits();
					case 64:return sourceRegister.GetValue();
					default:NeverOrAssert(false);
				}
				}();

		}
		else if (instruction.Operand1.Type == INSTRUCTIONS::OPERANDS::OperandType::Immediate) {
			sourceVal = std::bit_cast<uint64_t>(std::get<INSTRUCTIONS::OPERANDS::ImmediateOperand>(instruction.Operand1.Data).Value);
		}
		else {
			NeverOrAssert(false);
		}

		const auto& destinationOperand = std::get<INSTRUCTIONS::OPERANDS::RegisterOperand>(instruction.Operand0.Data);
		assert(destinationOperand.Flags & std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister));//the generic add handler only works for gprs
		 REGISTERS::GPR& destinationRegister = static_cast<REGISTERS::GPR&>(*std::bit_cast<REGISTERS::Register*>(destinationOperand.RegisterPointer));
		 switch (destinationOperand.SizeBits)
		 {
			 case 8: {
				 if (destinationOperand.Flags & std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister)) {
					 destinationRegister.SetHigh8Bits(destinationRegister.GetHigh8Bits() + static_cast<uint8_t>(sourceVal));
				 }
				 else {

					 destinationRegister.SetLow8Bits(destinationRegister.GetLow8Bits() + static_cast<uint8_t>(sourceVal));
				 }
				 break;
			 }
			 case 16:destinationRegister.SetLow16Bits(destinationRegister.GetLow16Bits() + static_cast<uint16_t>(sourceVal));
			 case 32:destinationRegister.SetLow32Bits(destinationRegister.GetLow32Bits() + static_cast<uint32_t>(sourceVal));
			 case 64:destinationRegister.SetValue(destinationRegister.GetValue() + sourceVal);
			 default:NeverOrAssert(false);
		 }



		



	}
	inline void Handle_SUB(VirtualCore& core, const  INSTRUCTIONS::Instruction& instruction) {
		ZoneNamed(HandleSub, true);//NOLINT
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