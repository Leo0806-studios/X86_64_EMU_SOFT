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
#include <intrin.h>
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
			const auto& sourceOperand = std::get<INSTRUCTIONS::OPERANDS::RegisterOperand>(instruction.Operand1.Data);
			assert(sourceOperand.Flags & std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister));//the generic add handler only works for gprs
			const REGISTERS::GPR& sourceRegister = static_cast<REGISTERS::GPR&>(*std::bit_cast<REGISTERS::Register*>(sourceOperand.RegisterPointer));
			sourceVal = [&]() -> uint64_t {
				switch (sourceOperand.SizeBits) {
					case 8: {
						if (sourceOperand.Flags & std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister)) {
							RunIfFullTraceMode(PrintRegister("Source Register", sourceRegister, "Before Execution:", 8, true, sourceRegister.GetHigh8Bits(), int8_t));
							return sourceRegister.GetHigh8Bits();
						}
						RunIfFullTraceMode(PrintRegister("Source Register", sourceRegister, "Before Execution:", 8, false, sourceRegister.GetLow8Bits(), int8_t));
						return sourceRegister.GetLow8Bits();
					}
					case 16:
						RunIfFullTraceMode(PrintRegister("Source Register", sourceRegister, "Before Execution:", 16, false, sourceRegister.GetLow16Bits(), int16_t));
						return sourceRegister.GetLow16Bits();
					case 32:
						RunIfFullTraceMode(PrintRegister("Source Register", sourceRegister, "Before Execution:", 32, false, sourceRegister.GetLow32Bits(), int32_t));
						return sourceRegister.GetLow32Bits();
					case 64:
						RunIfFullTraceMode(PrintRegister("Source Register", sourceRegister, "Before Execution:", 64, false, sourceRegister.GetValue(), int64_t));
						return sourceRegister.GetValue();
					default:NeverOrAssert(false);
				}
				__assume(false);
				}();

		}
		else if (instruction.Operand1.Type == INSTRUCTIONS::OPERANDS::OperandType::Immediate) {
			sourceVal = std::bit_cast<uint64_t>(std::get<INSTRUCTIONS::OPERANDS::ImmediateOperand>(instruction.Operand1.Data).Value);
			RunIfFullTraceMode(PrintImmediate("Source Immediate Value", sourceVal, std::get<INSTRUCTIONS::OPERANDS::ImmediateOperand>(instruction.Operand1.Data).SizeBits););


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
					RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "Before Execution:", 8, true, destinationRegister.GetHigh8Bits(), int8_t));
					destinationRegister.SetHigh8Bits(static_cast<uint8_t>(destinationRegister.GetHigh8Bits() + static_cast<uint8_t>(sourceVal)));
					RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "After Execution:", 8, true, destinationRegister.GetHigh8Bits(), int8_t));

				}
				else {

					destinationRegister.SetLow8Bits(static_cast<uint8_t>(destinationRegister.GetLow8Bits() + static_cast<uint8_t>(sourceVal)));
				}
				break;
			}
			case 16: {
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "Before Execution:", 16, false, destinationRegister.GetLow16Bits(), int16_t));
				destinationRegister.SetLow16Bits(static_cast<uint16_t>(destinationRegister.GetLow16Bits() + static_cast<uint16_t>(sourceVal)));
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "After Execution:", 16, false, destinationRegister.GetLow16Bits(), int16_t));
				break;
			}
			case 32: {
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "Before Execution:", 32, false, destinationRegister.GetLow32Bits(), int32_t));
				destinationRegister.SetLow32Bits(destinationRegister.GetLow32Bits() + static_cast<uint32_t>(sourceVal));
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "After Execution:", 32, false, destinationRegister.GetLow32Bits(), int32_t));
				break;
			}
			case 64:
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "Before Execution:", 64, false, destinationRegister.GetValue(), int64_t));
				destinationRegister.SetValue(destinationRegister.GetValue() + sourceVal);
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "After Execution:", 64, false, destinationRegister.GetValue(), int64_t));
				break;
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
			const auto& sourceOperand = std::get<INSTRUCTIONS::OPERANDS::RegisterOperand>(instruction.Operand1.Data);
			assert(sourceOperand.Flags & std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister));//the generic add handler only works for gprs
			const REGISTERS::GPR& sourceRegister = static_cast<REGISTERS::GPR&>(*std::bit_cast<REGISTERS::Register*>(sourceOperand.RegisterPointer));
			sourceVal = [&]() -> uint64_t {
				switch (sourceOperand.SizeBits) {
					case 8: {
						if (sourceOperand.Flags & std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister)) {
							RunIfFullTraceMode(PrintRegister("Source Register", sourceRegister, "Before Execution:", 8, true, sourceRegister.GetHigh8Bits(), int8_t));
							return sourceRegister.GetHigh8Bits();
						}
						RunIfFullTraceMode(PrintRegister("Source Register", sourceRegister, "Before Execution:", 8, false, sourceRegister.GetLow8Bits(), int8_t));
						return sourceRegister.GetLow8Bits();
					}
					case 16:
						RunIfFullTraceMode(PrintRegister("Source Register", sourceRegister, "Before Execution:", 16, false, sourceRegister.GetLow16Bits(), int16_t));
						return sourceRegister.GetLow16Bits();
					case 32:
						RunIfFullTraceMode(PrintRegister("Source Register", sourceRegister, "Before Execution:", 32, false, sourceRegister.GetLow32Bits(), int32_t));
						return sourceRegister.GetLow32Bits();
					case 64:
						RunIfFullTraceMode(PrintRegister("Source Register", sourceRegister, "Before Execution:", 64, false, sourceRegister.GetValue(), int64_t));
						return sourceRegister.GetValue();
					default:NeverOrAssert(false);
				}
				__assume(false);
				}();

		}
		else if (instruction.Operand1.Type == INSTRUCTIONS::OPERANDS::OperandType::Immediate) {
			sourceVal = std::bit_cast<uint64_t>(std::get<INSTRUCTIONS::OPERANDS::ImmediateOperand>(instruction.Operand1.Data).Value);
			RunIfFullTraceMode(PrintImmediate("Source Immediate Value", sourceVal, std::get<INSTRUCTIONS::OPERANDS::ImmediateOperand>(instruction.Operand1.Data).SizeBits););

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
					RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "Before Execution:", 8, true, destinationRegister.GetHigh8Bits(), int8_t));
					destinationRegister.SetHigh8Bits(static_cast<uint8_t>(destinationRegister.GetHigh8Bits() - static_cast<uint8_t>(sourceVal)));
					RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "After Execution:", 8, true, destinationRegister.GetHigh8Bits(), int8_t));
				}
				else {
					RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "Before Execution:", 8, false, destinationRegister.GetLow8Bits(), int8_t));
					destinationRegister.SetLow8Bits(static_cast<uint8_t>(destinationRegister.GetLow8Bits() - static_cast<uint8_t>(sourceVal)));
					RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "After Execution:", 8, false, destinationRegister.GetLow8Bits(), int8_t));
				}
				break;
			}
			case 16:
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "Before Execution:", 16, false, destinationRegister.GetLow16Bits(), int16_t));
				destinationRegister.SetLow16Bits(static_cast<uint16_t>(destinationRegister.GetLow16Bits() - static_cast<uint16_t>(sourceVal))); 
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "After Execution:", 16, false, destinationRegister.GetLow16Bits(), int16_t));
				break;
			case 32:
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "Before Execution:", 32, false, destinationRegister.GetLow32Bits(), int32_t));
				destinationRegister.SetLow32Bits(destinationRegister.GetLow32Bits() - static_cast<uint32_t>(sourceVal));
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "After Execution:", 32, false, destinationRegister.GetLow32Bits(), int32_t));
				break;
			case 64:
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "Before Execution:", 64, false, destinationRegister.GetValue(), int64_t));
				destinationRegister.SetValue(destinationRegister.GetValue() - sourceVal);
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "After Execution:", 64, false, destinationRegister.GetValue(), int64_t));
				break;
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
			const auto& sourceOperand = std::get<INSTRUCTIONS::OPERANDS::RegisterOperand>(instruction.Operand1.Data);
			assert(sourceOperand.Flags & std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister));//the generic add handler only works for gprs
			const REGISTERS::GPR& sourceRegister = static_cast<REGISTERS::GPR&>(*std::bit_cast<REGISTERS::Register*>(sourceOperand.RegisterPointer));
			sourceVal = [&]() -> uint64_t {
				switch (sourceOperand.SizeBits) {
					case 8: {
						if (sourceOperand.Flags & std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister)) {
							RunIfFullTraceMode(PrintRegister("Source Register", sourceRegister, "Before Execution:", 8, true, sourceRegister.GetHigh8Bits(), int8_t));
							return sourceRegister.GetHigh8Bits();
						}
						RunIfFullTraceMode(PrintRegister("Source Register", sourceRegister, "Before Execution:", 8, false, sourceRegister.GetLow8Bits(), int8_t));
						return sourceRegister.GetLow8Bits();
					}
					case 16:
						RunIfFullTraceMode(PrintRegister("Source Register", sourceRegister, "Before Execution:", 16, false, sourceRegister.GetLow16Bits(), int16_t));
						return sourceRegister.GetLow16Bits();
					case 32:
						RunIfFullTraceMode(PrintRegister("Source Register", sourceRegister, "Before Execution:", 32, false, sourceRegister.GetLow32Bits(), int32_t));
						return sourceRegister.GetLow32Bits();
					case 64:
						RunIfFullTraceMode(PrintRegister("Source Register", sourceRegister, "Before Execution:", 64, false, sourceRegister.GetValue(), int64_t));
						return sourceRegister.GetValue();
					default:NeverOrAssert(false);
				}
				__assume(false);
				}();

		}
		else if (instruction.Operand1.Type == INSTRUCTIONS::OPERANDS::OperandType::Immediate) {
			sourceVal = std::bit_cast<uint64_t>(std::get<INSTRUCTIONS::OPERANDS::ImmediateOperand>(instruction.Operand1.Data).Value);
			RunIfFullTraceMode(PrintImmediate("Source Immediate Value", sourceVal, std::get<INSTRUCTIONS::OPERANDS::ImmediateOperand>(instruction.Operand1.Data).SizeBits););

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
					RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "Before Execution:", 8, true, destinationRegister.GetHigh8Bits(), int8_t));
					destinationRegister.SetHigh8Bits(static_cast<uint8_t>(destinationRegister.GetHigh8Bits() | static_cast<uint8_t>(sourceVal)));
					RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "After Execution:", 8, true, destinationRegister.GetHigh8Bits(), int8_t));
				}
				else {
					RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "Before Execution:", 8, false, destinationRegister.GetLow8Bits(), int8_t));
					destinationRegister.SetLow8Bits(static_cast<uint8_t>(destinationRegister.GetLow8Bits() | static_cast<uint8_t>(sourceVal)));
					RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "After Execution:", 8, false, destinationRegister.GetLow8Bits(), int8_t));
				}
				break;
			}
			case 16:
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "Before Execution:", 16, false, destinationRegister.GetLow16Bits(), int16_t));
				destinationRegister.SetLow16Bits(static_cast<uint16_t>(destinationRegister.GetLow16Bits() | static_cast<uint16_t>(sourceVal))); 
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "After Execution:", 16, false, destinationRegister.GetLow16Bits(), int16_t));
				break;
			case 32:
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "Before Execution:", 32, false, destinationRegister.GetLow32Bits(), int32_t));
				destinationRegister.SetLow32Bits(destinationRegister.GetLow32Bits() | static_cast<uint32_t>(sourceVal)); 
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "After Execution:", 32, false, destinationRegister.GetLow32Bits(), int32_t));
				break;
			case 64:
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "Before Execution:", 64, false, destinationRegister.GetValue(), int64_t));
				destinationRegister.SetValue(destinationRegister.GetValue() | sourceVal); 
				RunIfFullTraceMode(PrintRegister("Destination Register", destinationRegister, "After Execution:", 64, false, destinationRegister.GetValue(), int64_t));
				break;
			default:NeverOrAssert(false); break;
		}
	}
}// namespace X86_64_EMU_SOFT::SYSTEM::CPU