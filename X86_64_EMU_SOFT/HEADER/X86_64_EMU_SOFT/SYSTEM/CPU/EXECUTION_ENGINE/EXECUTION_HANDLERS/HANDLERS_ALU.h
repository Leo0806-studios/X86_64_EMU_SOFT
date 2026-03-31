#pragma once
#include <bit>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <print>
#include <tuple>
#include <source_location>
#include <utility>
#include <variant>
#include <tracy/Tracy.hpp>
#include <SYSTEM/CPU/REGISTERS/GPR.h>
#include <SYSTEM/CPU/REGISTERS/REGISTER_BASE.h>
#include <HELPERS/MACROS.h>
#include "HELPERS/GLOBALS.h"
#include "SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h"
#include "SYSTEM/CPU/EXECUTION_ENGINE/EXECUTION_ENGINE.h"
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/VCORE.h"

namespace X86_64_EMU_SOFT::SYSTEM::CPU {
	inline void Handle_ADD(VirtualCore& core, const INSTRUCTIONS::Instruction& instruction) {
		ZoneScoped;//NOLINT
		std::ignore = core;
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
				__assume(false);
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
		auto& destinationRegister = static_cast<REGISTERS::GPR&>(*std::bit_cast<REGISTERS::Register*>(destinationOperand.RegisterPointer));
		switch (destinationOperand.SizeBits)
		{
			case 8: {
				if (destinationOperand.Flags & std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister)) {
					destinationRegister.SetHigh8Bits(static_cast<uint8_t>(destinationRegister.GetHigh8Bits() + static_cast<uint8_t>(sourceVal)));
				}
				else {

					destinationRegister.SetLow8Bits(static_cast<uint8_t>(destinationRegister.GetLow8Bits() + static_cast<uint8_t>(sourceVal)));
				}
				break;
			}
			case 16:destinationRegister.SetLow16Bits(static_cast<uint16_t>(destinationRegister.GetLow16Bits() + static_cast<uint16_t>(sourceVal))); break;
			case 32:destinationRegister.SetLow32Bits(destinationRegister.GetLow32Bits() + static_cast<uint32_t>(sourceVal)); break;
			case 64:destinationRegister.SetValue(destinationRegister.GetValue() + sourceVal); break;
			default:NeverOrAssert(false); break;
		}







	}
	inline void Handle_SUB(VirtualCore& core, const  INSTRUCTIONS::Instruction& instruction) {
		ZoneScoped;//NOLINT
		std::ignore = core;
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
				__assume(false);
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
		auto& destinationRegister = static_cast<REGISTERS::GPR&>(*std::bit_cast<REGISTERS::Register*>(destinationOperand.RegisterPointer));
		switch (destinationOperand.SizeBits)
		{
			case 8: {
				if (destinationOperand.Flags & std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister)) {
					destinationRegister.SetHigh8Bits(static_cast<uint8_t>(destinationRegister.GetHigh8Bits() - static_cast<uint8_t>(sourceVal)));
				}
				else {

					destinationRegister.SetLow8Bits(static_cast<uint8_t>(destinationRegister.GetLow8Bits() - static_cast<uint8_t>(sourceVal)));
				}
				break;
			}
			case 16:destinationRegister.SetLow16Bits(static_cast<uint16_t>(destinationRegister.GetLow16Bits() - static_cast<uint16_t>(sourceVal))); break;
			case 32:destinationRegister.SetLow32Bits(destinationRegister.GetLow32Bits() - static_cast<uint32_t>(sourceVal)); break;
			case 64:destinationRegister.SetValue(destinationRegister.GetValue() - sourceVal); break;
			default:NeverOrAssert(false); break;
		}
	}

	inline void Handle_OR(VirtualCore& core, const  INSTRUCTIONS::Instruction& instruction) {
		ZoneScoped;//NOLINT
		std::ignore = core;
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
				__assume(false);
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
		auto& destinationRegister = static_cast<REGISTERS::GPR&>(*std::bit_cast<REGISTERS::Register*>(destinationOperand.RegisterPointer));
		switch (destinationOperand.SizeBits)
		{
			case 8: {
				if (destinationOperand.Flags & std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister)) {
					destinationRegister.SetHigh8Bits(static_cast<uint8_t>(destinationRegister.GetHigh8Bits() | static_cast<uint8_t>(sourceVal)));
				}
				else {

					destinationRegister.SetLow8Bits(static_cast<uint8_t>(destinationRegister.GetLow8Bits() | static_cast<uint8_t>(sourceVal)));
				}
				break;
			}
			case 16:destinationRegister.SetLow16Bits(static_cast<uint16_t>(destinationRegister.GetLow16Bits() | static_cast<uint16_t>(sourceVal))); break;
			case 32:destinationRegister.SetLow32Bits(destinationRegister.GetLow32Bits() | static_cast<uint32_t>(sourceVal)); break;
			case 64:destinationRegister.SetValue(destinationRegister.GetValue() | sourceVal); break;
			default:NeverOrAssert(false); break;
		}
	}
}// namespace X86_64_EMU_SOFT::SYSTEM::CPU