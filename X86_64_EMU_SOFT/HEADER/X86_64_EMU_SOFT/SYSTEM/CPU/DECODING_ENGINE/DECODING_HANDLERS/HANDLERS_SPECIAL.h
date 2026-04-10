#pragma once
#include <cstdint>
#include <tuple>
#include <utility>
#include <tracy/Tracy.hpp>
#include <bit>
#include <array>
#include <cassert>
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_ENGINE.h"
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/INSTRUCTIONS/OPCODE_BYTES.h"
#include "SYSTEM/CPU/VCORE.h"
#include "SYSTEM/MEMORY/MEMORY.h"
#include <HELPERS/MACROS.h>
#include <SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h>



namespace X86_64_EMU_SOFT::SYSTEM::CPU {
	inline DEFINE_HANDLER(Handle_MOV_rm16rm32rm64_r16r32r64_0x89) {
		ZoneScoped;//NOLINT
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;
		instruction.Type = INSTRUCTIONS::InstructionType::MOV;
		uint8_t operandSize = core.GetDefaultOperandSize();
		assert(std::bit_cast<uint8_t>(INSTRUCTIONS::REX()) == false);
		static_assert(!std::bit_cast<uint8_t>(INSTRUCTIONS::REX().reserved));//reserved is guaranteed to be 0100 for a present REX prefix (true)
		if (std::bit_cast<bool>(prefixes.RexPrefix) && prefixes.RexPrefix.W) {
			operandSize = 64;
		}
		else {
			if (operandSize == 32 && prefixes.OperandSizeOverride) {
				operandSize = 16;
			}
			else if (operandSize == 16 && prefixes.OperandSizeOverride) {
				operandSize = 32;
			}
		}
		DigestModrmSib(ModrmSib, hasSIB);
		std::ignore = hasSIB;
		const INSTRUCTIONS::TargetRegister sourceRegister = DecodingEngine::DecodeTargetRegister(static_cast<uint8_t>(ModrmSib.first.reg | static_cast<uint8_t>(prefixes.RexPrefix.R << 3ULL)));
		const auto sourceOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
			.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(sourceRegister)),
			.SizeBits = operandSize,
			.Flags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)
		};
		instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = sourceOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		if (ModrmSib.first.mod == 0b11) {
			const INSTRUCTIONS::TargetRegister destinationRegister = DecodingEngine::DecodeTargetRegister(static_cast<uint8_t>(ModrmSib.first.rm | static_cast<uint8_t>(prefixes.RexPrefix.B << 3ULL)));
			const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
				.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationRegister)),
				.SizeBits = operandSize,
				.Flags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)
			};
			instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		}
		else {
			throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands are not yet supported for MOV 0x89");
		}
		instruction.OperandCount = 2;
		return true;
	}


	inline DEFINE_HANDLER(Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE) {
		ZoneScoped;//NOLINT
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;
		instruction.Type = INSTRUCTIONS::InstructionType::MOV;

		uint8_t operandSize = core.GetDefaultOperandSize();
		assert(std::bit_cast<uint8_t>(INSTRUCTIONS::REX()) == false);
		static_assert(!std::bit_cast<uint8_t>(INSTRUCTIONS::REX().reserved));//reserved is guaranteed to be 0100 for a present REX prefix (true)
		INSTRUCTIONS::OPERANDS::ImmediateOperand sourceOperand{
		   .Value = {0,0,0,0,0,0,0,0},//will be set later
		   .SizeBits = 0,//will be set later
		};
		if (std::bit_cast<bool>(prefixes.RexPrefix) && prefixes.RexPrefix.W) {
			operandSize = 64;
			sourceOperand.SizeBits = 64;

		}
		else {
			if (operandSize == 32 && prefixes.OperandSizeOverride) {
				operandSize = 16;
				sourceOperand.SizeBits = 16;

			}
			else if (operandSize == 16 && prefixes.OperandSizeOverride) {
				operandSize = 32;
				sourceOperand.SizeBits = 32;

			} 
			if (operandSize == 32) {
				const std::array<uint8_t, 4> immediateValue = std::bit_cast<std::array<uint8_t, 4>>(static_cast<uint32_t>(core.FetchBytes(address, 4)));
				sourceOperand.Value[0] = immediateValue[0];
				sourceOperand.Value[1] = immediateValue[1];
				sourceOperand.Value[2] = immediateValue[2];
				sourceOperand.Value[3] = immediateValue[3];
				instruction.InstructionLengthBytes += 4;
			}
			else if (operandSize == 16) {
				const std::array<uint8_t, 2> immediateValue = std::bit_cast<std::array<uint8_t, 2>>(static_cast<uint16_t>(core.FetchBytes(address, 2)));
				sourceOperand.Value[0] = immediateValue[0];
				sourceOperand.Value[1] = immediateValue[1];
				instruction.InstructionLengthBytes += 2;
			}
			else {
				NeverOrAssert(false);
			}
		}
		if (operandSize == 64) {
			const auto immediateValue = core.FetchBytes(address, 8);
			sourceOperand.Value = std::bit_cast<std::array<uint8_t, 8>>(immediateValue);
			instruction.InstructionLengthBytes += 8;
		}
		const INSTRUCTIONS::TargetRegister destinationRegister = DecodingEngine::GetTargetRegisterfromAdditiveID(static_cast<uint8_t>(byte & static_cast<uint8_t>(0b111)));
		const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
			.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationRegister)),
			.SizeBits = operandSize,
			.Flags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)
		};
		instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = sourceOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Immediate };
		return true;
	}
	

	inline  DEFINE_HANDLER(Handle_NOP_0x90) {//NOSONAR

		ZoneScoped;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;

		std::ignore = core;
		std::ignore = address;
		std::ignore = prefixes;
		
		instruction.Type = INSTRUCTIONS::InstructionType::NOP;

		return true;
	}
	inline DEFINE_HANDLER(Handle_UD_0xD6) {//NOSONAR
		ZoneScoped;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;

		std::ignore = core;
		std::ignore = address;
		std::ignore = prefixes;
		
		instruction.Type = INSTRUCTIONS::InstructionType::UD;
		return true;
	}


}//namespace X86_64_EMU_SOFT::SYSTEM::CPU