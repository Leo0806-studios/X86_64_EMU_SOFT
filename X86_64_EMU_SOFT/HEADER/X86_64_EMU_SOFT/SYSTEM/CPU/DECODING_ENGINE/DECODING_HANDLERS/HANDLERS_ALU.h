#pragma once
#include <cstdint>
#include <tuple>
#include <utility>
#include <array>
#include <ios>
#include <sstream>
#include <cstring>
#include <bit>

#include <cassert>
#include <tracy/Tracy.hpp>
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_ENGINE.h"
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/INSTRUCTIONS/OPCODE_BYTES.h"
#include "SYSTEM/CPU/VCORE.h"
#include "SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h"
#include <HELPERS/MACROS.h>


namespace X86_64_EMU_SOFT::SYSTEM::CPU {
	inline DEFINE_HANDLER(Handle_ADD_rm8_r8_0x0) {
		ZoneScoped;//NOLINT
		instruction.Type = INSTRUCTIONS::InstructionType::ADD;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;



		DigestModrmSib(ModrmSib, hasSIB);
		std::ignore = hasSIB;
		assert(std::bit_cast<uint8_t>(INSTRUCTIONS::REX()) == false);
		const auto sourceRegister = DecodingEngine::DecodeTarget8BitRegister(core, static_cast<uint8_t>((ModrmSib.first.reg) | static_cast<uint8_t>(prefixes.RexPrefix.R << 3ULL)));
		const uint8_t sourceFlags = static_cast<uint8_t>(std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
			(IsHighRegister(sourceRegister) ? std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister) : static_cast<uint8_t>(0U)));
		const INSTRUCTIONS::OPERANDS::RegisterOperand SourceOperand =
		{
			.RegisterPointer = std::bit_cast<std::array<uint8_t,8>>(&core.GetRegister(sourceRegister)),
			.SizeBits = 8,
			.Flags = sourceFlags
		};
		instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = SourceOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };


		if (ModrmSib.first.mod == 3) {
			const auto destinationregister = DecodingEngine::DecodeTarget8BitRegister(core, static_cast<uint8_t>(ModrmSib.first.rm | static_cast<uint8_t> (prefixes.RexPrefix.B << 3ULL)));
			const uint8_t destinationFlags = static_cast<uint8_t>(std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
				(IsHighRegister(destinationregister) ? std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister) : static_cast<uint8_t>(0U)));
			const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
				.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationregister)),
				.SizeBits = 8,
				.Flags = destinationFlags
			};
			instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		}
		else {
			throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands are not yet supported for ADD 0x0");
		}
		instruction.OperandCount = 2;
		return true;
	}


	inline  DEFINE_HANDLER(Handle_ADD_rm16rm32rm64_r16r32r64_0x1) {
		ZoneScoped;//NOLINT
		instruction.Type = INSTRUCTIONS::InstructionType::ADD;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;


		DigestModrmSib(ModrmSib, hasSIB);
		std::ignore = hasSIB;

		uint8_t operandSize = core.GetDefaultOperandSize();
		assert(std::bit_cast<uint8_t>(INSTRUCTIONS::REX()) == false);
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

		const INSTRUCTIONS::TargetRegister sourceRegister = DecodingEngine::DecodeTargetRegister(static_cast<uint8_t>(ModrmSib.first.reg | static_cast<uint8_t>(prefixes.RexPrefix.R << 3ULL)));
		const INSTRUCTIONS::OPERANDS::RegisterOperand SourceOperand
		{
			.RegisterPointer = std::bit_cast<std::array<uint8_t,8>>(&core.GetRegister(sourceRegister)),
			.SizeBits = operandSize,
			.Flags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)
		};
		instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = SourceOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };


		if (ModrmSib.first.mod == 3) {
			const INSTRUCTIONS::TargetRegister destinationRegister = DecodingEngine::DecodeTargetRegister(static_cast<uint8_t>(ModrmSib.first.rm | static_cast<uint8_t> (prefixes.RexPrefix.B << 3ULL)));
			const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
				.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationRegister)),
				.SizeBits = operandSize,
				.Flags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)
			};
			instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		}
		else {
			throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands not supported yet for ADD 0x1 decoding");
		}
		instruction.OperandCount = 2;
		return true;
	}
	inline DEFINE_HANDLER(Handle_ADD_r8_rm8_0x2) {
		ZoneScoped;//NOLINT
		instruction.Type = INSTRUCTIONS::InstructionType::ADD;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;




		DigestModrmSib(ModrmSib, hasSIB);
		std::ignore = hasSIB;
		assert(std::bit_cast<uint8_t>(INSTRUCTIONS::REX()) == false);

		const auto destinationRegister = DecodingEngine::DecodeTarget8BitRegister(core, static_cast<uint8_t>(ModrmSib.first.rm | static_cast<uint8_t>(prefixes.RexPrefix.B << 3ULL)));
		const uint8_t destinationFlags = static_cast<uint8_t>(std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
			(IsHighRegister(destinationRegister) ? std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister) : static_cast<uint8_t>(0U)));
		const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
			.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationRegister)),
			.SizeBits = 8,
			.Flags = destinationFlags
		};
		instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		if (ModrmSib.first.mod == 0b11) {
			const auto sourceRegister = DecodingEngine::DecodeTarget8BitRegister(core, static_cast<uint8_t>(ModrmSib.first.reg | static_cast<uint8_t>(prefixes.RexPrefix.R << 3ULL)));
			const uint8_t sourceFlags = static_cast<uint8_t>(std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
				(IsHighRegister(destinationRegister) ? std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister) : static_cast<uint8_t>(0U)));
			const auto sourceOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
				.RegisterPointer = std::bit_cast<std::array<uint8_t,8>>(&core.GetRegister(sourceRegister)),
				.SizeBits = 8,
				.Flags = sourceFlags
			};
			instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = sourceOperand,.Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		}
		else {
			throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands are not yet supported for ADD 0x2");
		}
		return true;
	}

	inline DEFINE_HANDLER(Handle_ADD_r16r32r64_rm16rm32rm64_0x3) {
		ZoneScoped;//NOLINT
		instruction.Type = INSTRUCTIONS::InstructionType::ADD;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;


		DigestModrmSib(ModrmSib, hasSIB);
		std::ignore = hasSIB;

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

		const INSTRUCTIONS::TargetRegister destinationRegister = DecodingEngine::DecodeTargetRegister(static_cast<uint8_t>(ModrmSib.first.reg | static_cast<uint8_t>(prefixes.RexPrefix.R << 3ULL)));
		const INSTRUCTIONS::OPERANDS::RegisterOperand destinationOperand
		{
			.RegisterPointer = std::bit_cast<std::array<uint8_t,8>>(&core.GetRegister(destinationRegister)),
			.SizeBits = operandSize,
			.Flags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)
		};
		instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };


		if (ModrmSib.first.mod == 3) {
			const INSTRUCTIONS::TargetRegister sourceRegister = DecodingEngine::DecodeTargetRegister(static_cast<uint8_t>(ModrmSib.first.rm | static_cast<uint8_t> (prefixes.RexPrefix.B << 3ULL)));
			const auto sourceOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
				.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(sourceRegister)),
				.SizeBits = operandSize,
				.Flags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)
			};
			instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = sourceOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		}
		else {
			throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands not supported yet for ADD 0x1 decoding");
		}
		instruction.OperandCount = 2;
		return true;

	}
	inline DEFINE_HANDLER(Handle_ADD_AL_imm8_0x4) {
		ZoneScoped;//NOLINT
		std::ignore = prefixes;
		instruction.Type = INSTRUCTIONS::InstructionType::ADD;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;
		const auto destinationRegister = INSTRUCTIONS::TargetRegister::RAX;
		const uint8_t destinationFlags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister);
		const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
			.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationRegister)),
			.SizeBits = 8,
			.Flags = destinationFlags
		};
		instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		const auto immediateValue = static_cast<uint8_t>(core.FetchBytes(address, 1));
		address += 1;
		instruction.InstructionLengthBytes++;
		const auto immediateOperand = INSTRUCTIONS::OPERANDS::ImmediateOperand{
			.Value = {immediateValue,0,0,0,0,0,0,0},
			.SizeBits = 8
		};
		instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = immediateOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Immediate };
		instruction.OperandCount = 2;
		return true;
	}
	inline DEFINE_HANDLER(Handle_ADD_AxEaxRax_imm16imm32_0x5) {
		ZoneScoped;//NOLINT
		std::ignore = prefixes;
		instruction.Type = INSTRUCTIONS::InstructionType::ADD;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;


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


		const auto destinationRegister = INSTRUCTIONS::TargetRegister::RAX;
		const uint8_t destinationFlags = static_cast<uint8_t>(std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
			std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister));
		const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
			.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationRegister)),
			.SizeBits = operandSize,
			.Flags = destinationFlags
		};
		instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		if (operandSize == 16) {
			const auto immediateValue = std::bit_cast<std::array<uint8_t, 2>>(static_cast<uint16_t>(core.FetchBytes(address, 2)));
			address += 2;
			instruction.InstructionLengthBytes += 2;
			const auto immediateOperand = INSTRUCTIONS::OPERANDS::ImmediateOperand{
				.Value = {immediateValue[0],immediateValue[1],0,0,0,0,0,0},
				.SizeBits = 32
			};
			instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = immediateOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Immediate };
		}
		else if (operandSize == 32) {
			const auto immediateValue = std::bit_cast<std::array<uint8_t, 4>>(static_cast<uint32_t>(core.FetchBytes(address, 4)));
			address += 4;
			instruction.InstructionLengthBytes += 4;
			const auto immediateOperand = INSTRUCTIONS::OPERANDS::ImmediateOperand{
				.Value = {immediateValue[0],immediateValue[1],immediateValue[2],immediateValue[3],0,0,0,0},
				.SizeBits = 32
			};
			instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = immediateOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Immediate };
		}
		else if (operandSize == 64) {
			const auto immediateValue = static_cast<uint64_t>(static_cast<int64_t>(core.FetchBytes(address, 4)));
			address += 4;
			instruction.InstructionLengthBytes += 8;//sign extension takes up the rest of the 8 bytes. si its 4 bytes immediate + 4 bytes of sign extension
			const auto immediateOperand = INSTRUCTIONS::OPERANDS::ImmediateOperand{
				.Value = std::bit_cast<std::array<uint8_t,8>>(immediateValue),
				.SizeBits = 64
			};
			instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = immediateOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Immediate };
		}
		else {
			NeverOrAssert(false);
		}
		return true;
	}


	// TODO : insert handlers for 0x6,0x7

	inline DEFINE_HANDLER(Handle_OR_rm8_r8_0x8) {
		ZoneScoped;//NOLINT
		instruction.Type = INSTRUCTIONS::InstructionType::OR;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;

		DigestModrmSib(ModrmSib, hasSIB);
		std::ignore = hasSIB;
		assert(std::bit_cast<uint8_t>(INSTRUCTIONS::REX()) == false);
		const auto sourceRegister = DecodingEngine::DecodeTarget8BitRegister(core, static_cast<uint8_t>(ModrmSib.first.reg | static_cast<uint8_t>(prefixes.RexPrefix.R << 3ULL)));
		const uint8_t sourceFlags = static_cast<uint8_t>(std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
			(IsHighRegister(sourceRegister) ? std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister) : static_cast<uint8_t>(0U)));
		const INSTRUCTIONS::OPERANDS::RegisterOperand SourceOperand =
		{
			.RegisterPointer = std::bit_cast<std::array<uint8_t,8>>(&core.GetRegister(sourceRegister)),
			.SizeBits = 8,
			.Flags = sourceFlags
		};
		instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = SourceOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };


		if (ModrmSib.first.mod == 3) {
			const auto destinationregister = DecodingEngine::DecodeTarget8BitRegister(core, static_cast<uint8_t>(ModrmSib.first.rm | static_cast<uint8_t> (prefixes.RexPrefix.B << 3ULL)));
			const uint8_t destinationFlags = static_cast<uint8_t>(std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
				(IsHighRegister(destinationregister) ? std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister) : static_cast<uint8_t>(0U)));
			const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
				.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationregister)),
				.SizeBits = 8,
				.Flags = destinationFlags
			};
			instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		}
		else {
			throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands are not yet supported for ADD 0x0");
		}
		instruction.OperandCount = 2;
		return true;

	}

	inline  DEFINE_HANDLER(Handle_OR_rm16rm32rm64_r16r32r64_0x9) {
		ZoneScoped;//NOLINT
		instruction.Type = INSTRUCTIONS::InstructionType::OR;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;


		DigestModrmSib(ModrmSib, hasSIB);
		std::ignore = hasSIB;

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

		const INSTRUCTIONS::TargetRegister sourceRegister = DecodingEngine::DecodeTargetRegister(static_cast<uint8_t>(ModrmSib.first.reg | static_cast<uint8_t>(prefixes.RexPrefix.R << 3ULL)));
		const INSTRUCTIONS::OPERANDS::RegisterOperand SourceOperand
		{
			.RegisterPointer = std::bit_cast<std::array<uint8_t,8>>(&core.GetRegister(sourceRegister)),
			.SizeBits = operandSize,
			.Flags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)
		};
		instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = SourceOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };


		if (ModrmSib.first.mod == 3) {
			const INSTRUCTIONS::TargetRegister destinationregister = DecodingEngine::DecodeTargetRegister(static_cast<uint8_t>(ModrmSib.first.rm | static_cast<uint8_t> (prefixes.RexPrefix.B << 3ULL)));
			const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
				.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationregister)),
				.SizeBits = operandSize,
				.Flags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)
			};
			instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		}
		else {
			throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands not supported yet for ADD 0x1 decoding");
		}
		instruction.OperandCount = 2;
		return true;

	}

	inline DEFINE_HANDLER(Handle_OR_r8_rm8_0xA) {
		ZoneScoped;//NOLINT
		instruction.Type = INSTRUCTIONS::InstructionType::OR;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;


		DigestModrmSib(ModrmSib, hasSIB);
		std::ignore = hasSIB;
		assert(std::bit_cast<uint8_t>(INSTRUCTIONS::REX()) == false);

		const auto destinationRegister = DecodingEngine::DecodeTarget8BitRegister(core, static_cast<uint8_t>(ModrmSib.first.reg | static_cast<uint8_t>(prefixes.RexPrefix.R << 3ULL)));
		const uint8_t destinationFlags = static_cast<uint8_t>(std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
			(IsHighRegister(destinationRegister) ? std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister) : static_cast<uint8_t>(0U)));
		const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
			.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationRegister)),
			.SizeBits = 8,
			.Flags = destinationFlags
		};
		instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		if (ModrmSib.first.mod == 0b11) {
			const auto sourceRegister = DecodingEngine::DecodeTarget8BitRegister(core, static_cast<uint8_t>(ModrmSib.first.rm | static_cast<uint8_t>(prefixes.RexPrefix.B << 3ULL)));
			const uint8_t sourceFlags = static_cast<uint8_t>(std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
				(IsHighRegister(destinationRegister) ? std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister) : static_cast<uint8_t>(0U)));
			const auto sourceOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
				.RegisterPointer = std::bit_cast<std::array<uint8_t,8>>(&core.GetRegister(sourceRegister)),
				.SizeBits = 8,
				.Flags = sourceFlags
			};
			instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = sourceOperand,.Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		}
		else {
			throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands are not yet supported for ADD 0x2");
		}
		return true;
	}


	inline DEFINE_HANDLER(Handle_OR_r16r32r64_rm16rm32rm64_0xB) {
		ZoneScoped;//NOLINT
		instruction.Type = INSTRUCTIONS::InstructionType::OR;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;


		DigestModrmSib(ModrmSib, hasSIB);
		std::ignore = hasSIB;

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

		const INSTRUCTIONS::TargetRegister destinationRegister = DecodingEngine::DecodeTargetRegister(static_cast<uint8_t>(ModrmSib.first.reg | static_cast<uint8_t>(prefixes.RexPrefix.R << 3ULL)));
		const INSTRUCTIONS::OPERANDS::RegisterOperand destinationOperand
		{
			.RegisterPointer = std::bit_cast<std::array<uint8_t,8>>(&core.GetRegister(destinationRegister)),
			.SizeBits = operandSize,
			.Flags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)
		};
		instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };


		if (ModrmSib.first.mod == 3) {
			const INSTRUCTIONS::TargetRegister sourceRegister = DecodingEngine::DecodeTargetRegister(static_cast<uint8_t>(ModrmSib.first.rm | static_cast<uint8_t> (prefixes.RexPrefix.B << 3ULL)));
			const auto sourceOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
				.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(sourceRegister)),
				.SizeBits = operandSize,
				.Flags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)
			};
			instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = sourceOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		}
		else {
			throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands not supported yet for ADD 0x1 decoding");
		}
		instruction.OperandCount = 2;
		return true;
	}

	inline DEFINE_HANDLER(Handle_OR_AL_imm8_0xC) {
		ZoneScoped;//NOLINT
		std::ignore = prefixes;
		instruction.Type = INSTRUCTIONS::InstructionType::OR;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;
		const auto destinationRegister = INSTRUCTIONS::TargetRegister::RAX;
		const uint8_t destinationFlags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister);
		const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
			.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationRegister)),
			.SizeBits = 8,
			.Flags = destinationFlags
		};
		instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		const auto immediateValue = static_cast<uint8_t>(core.FetchBytes(address, 1));
		address += 1;
		instruction.InstructionLengthBytes++;
		const auto immediateOperand = INSTRUCTIONS::OPERANDS::ImmediateOperand{
			.Value = {immediateValue,0,0,0,0,0,0,0},
			.SizeBits = 8
		};
		instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = immediateOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Immediate };
		instruction.OperandCount = 2;
		return true;
	}

	inline DEFINE_HANDLER(Handle_OR_AxEaxRax_imm16imm32_0xD) {
		ZoneScoped;//NOLINT
		std::ignore = prefixes;
		instruction.Type = INSTRUCTIONS::InstructionType::OR;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;


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


		const auto destinationRegister = INSTRUCTIONS::TargetRegister::RAX;
		const uint8_t destinationFlags = static_cast<uint8_t>(std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
			std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister));
		const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
			.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationRegister)),
			.SizeBits = operandSize,
			.Flags = destinationFlags
		};
		instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		if (operandSize == 16) {
			const auto immediateValue = std::bit_cast<std::array<uint8_t, 2>>(static_cast<uint16_t>(core.FetchBytes(address, 2)));
			address += 2;
			instruction.InstructionLengthBytes += 2;
			const auto immediateOperand = INSTRUCTIONS::OPERANDS::ImmediateOperand{
				.Value = {immediateValue[0],immediateValue[1],0,0,0,0,0,0},
				.SizeBits = 32
			};
			instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = immediateOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Immediate };
		}
		else if (operandSize == 32) {
			const auto immediateValue = std::bit_cast<std::array<uint8_t, 4>>(static_cast<uint32_t>(core.FetchBytes(address, 4)));
			address += 4;
			instruction.InstructionLengthBytes += 4;
			const auto immediateOperand = INSTRUCTIONS::OPERANDS::ImmediateOperand{
				.Value = {immediateValue[0],immediateValue[1],immediateValue[2],immediateValue[3],0,0,0,0},
				.SizeBits = 32
			};
			instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = immediateOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Immediate };
		}
		else if (operandSize == 64) {
			const auto immediateValue = static_cast<uint64_t>(static_cast<int64_t>(core.FetchBytes(address, 4)));
			address += 4;
			instruction.InstructionLengthBytes += 8;//sign extension takes up the rest of the 8 bytes. si its 4 bytes immediate + 4 bytes of sign extension
			const auto immediateOperand = INSTRUCTIONS::OPERANDS::ImmediateOperand{
				.Value = std::bit_cast<std::array<uint8_t,8>>(immediateValue),
				.SizeBits = 64
			};
			instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = immediateOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Immediate };
		}
		else {
			NeverOrAssert(false);
		}
		return true;
	}

	//TODO: insert handlers from 0xE to 0x27

	inline DEFINE_HANDLER(Handle_SUB_rm8_r8_0x28) {
		ZoneScoped;//NOLINT
		instruction.Type = INSTRUCTIONS::InstructionType::SUB;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;
		DigestModrmSib(ModrmSib, hasSIB);
		std::ignore = hasSIB;
		assert(std::bit_cast<uint8_t>(INSTRUCTIONS::REX()) == false);
		const auto sourceRegister = DecodingEngine::DecodeTarget8BitRegister(core, static_cast<uint8_t>(ModrmSib.first.reg | static_cast<uint8_t>(prefixes.RexPrefix.R << 3ULL)));
		const uint8_t sourceFlags = static_cast<uint8_t>(std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
			(IsHighRegister(sourceRegister) ? std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister) : static_cast<uint8_t>(0U)));
		const INSTRUCTIONS::OPERANDS::RegisterOperand SourceOperand =
		{
			.RegisterPointer = std::bit_cast<std::array<uint8_t,8>>(&core.GetRegister(sourceRegister)),
			.SizeBits = 8,
			.Flags = sourceFlags
		};
		instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = SourceOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };

		if (ModrmSib.first.mod == 0b11) {
			const auto destinationRegister = DecodingEngine::DecodeTarget8BitRegister(core, static_cast<uint8_t>(ModrmSib.first.rm | static_cast<uint8_t>(prefixes.RexPrefix.B << 3ULL)));
			const uint8_t destinationFlags = static_cast<uint8_t>(std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
				(IsHighRegister(destinationRegister) ? std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister) : static_cast<uint8_t>(0U)));
			const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
				.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationRegister)),
				.SizeBits = 8,
				.Flags = destinationFlags
			};
			instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		}
		else {
			throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands are not yet supported for SUB 0x28");
		}
		return true;
	}

	inline DEFINE_HANDLER(Handle_SUB_rm16rm32rm64_r16r32r64_0x29) {
		ZoneScoped;//NOLINT
		instruction.Type = INSTRUCTIONS::InstructionType::SUB;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;

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
		const INSTRUCTIONS::OPERANDS::RegisterOperand SourceOperand
		{
			.RegisterPointer = std::bit_cast<std::array<uint8_t,8>>(&core.GetRegister(sourceRegister)),
			.SizeBits = operandSize,
			.Flags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)
		};
		instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = SourceOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
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
			throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands not supported yet for SUB 0x29 decoding");
		}
		return true;
	}
	inline DEFINE_HANDLER(Handle_SUB_r8_rm8_0x2A) {
		ZoneScoped;//NOLINT
		instruction.Type = INSTRUCTIONS::InstructionType::SUB;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;
		DigestModrmSib(ModrmSib, hasSIB);
		std::ignore = hasSIB;
		INSTRUCTIONS::TargetRegister destinationRegister = DecodingEngine::DecodeTarget8BitRegister(core, static_cast<uint8_t>(ModrmSib.first.reg | static_cast<uint8_t>(prefixes.RexPrefix.R << 3ULL)));
		const uint8_t destinationFlags = static_cast<uint8_t>(std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
			(IsHighRegister(destinationRegister) ? std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister) : static_cast<uint8_t>(0U)));
		const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
			.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationRegister)),
			.SizeBits = 8,
			.Flags = destinationFlags
		};
		instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		if (ModrmSib.first.mod == 0b11) {
			const auto sourceRegister = DecodingEngine::DecodeTarget8BitRegister(core, static_cast<uint8_t>(ModrmSib.first.rm | static_cast<uint8_t>(prefixes.RexPrefix.B << 3ULL)));
			const uint8_t sourceFlags = static_cast<uint8_t>(std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
				(IsHighRegister(destinationRegister) ? std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister) : static_cast<uint8_t>(0U)));
			const auto sourceOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
				.RegisterPointer = std::bit_cast<std::array<uint8_t,8>>(&core.GetRegister(sourceRegister)),
				.SizeBits = 8,
				.Flags = sourceFlags
			};
			instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = sourceOperand,.Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		}
		else {
			throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands are not yet supported for SUB 0x2A");
		}
		return true;
	}

	inline DEFINE_HANDLER(Handle_SUB_r16r32r64_rm16rm32rm64_0x2B) {
		ZoneScoped;//NOLINT
		instruction.Type = INSTRUCTIONS::InstructionType::SUB;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;
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
		const INSTRUCTIONS::TargetRegister destinationRegister = DecodingEngine::DecodeTargetRegister(static_cast<uint8_t>(ModrmSib.first.reg | static_cast<uint8_t>(prefixes.RexPrefix.R << 3ULL)));
		const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
			.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationRegister)),
			.SizeBits = operandSize,
			.Flags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)
		};
		instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		if (ModrmSib.first.mod == 0b11) {
			const INSTRUCTIONS::TargetRegister sourceRegister = DecodingEngine::DecodeTargetRegister(static_cast<uint8_t>(ModrmSib.first.rm | static_cast<uint8_t>(prefixes.RexPrefix.B << 3ULL)));
			const auto sourceOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
				.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(sourceRegister)),
				.SizeBits = operandSize,
				.Flags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)
			};
			instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = sourceOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		}
		else {
			throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands not supported yet for SUB 0x2B decoding");
		}
		return true;
	}


	inline DEFINE_HANDLER(Handle_SUB_AL_imm8_0x2C) {
		ZoneScoped;//NOLINT
		std::ignore = prefixes;
		instruction.Type = INSTRUCTIONS::InstructionType::SUB;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;
		const auto destinationRegister = INSTRUCTIONS::TargetRegister::RAX;
		const uint8_t destinationFlags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister);
		const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
			.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationRegister)),
			.SizeBits = 8,
			.Flags = destinationFlags
		};
		instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		const auto immediateValue = static_cast<uint8_t>(core.FetchBytes(address, 1));
		address += 1;
		instruction.InstructionLengthBytes++;
		const auto immediateOperand = INSTRUCTIONS::OPERANDS::ImmediateOperand{
			.Value = {immediateValue,0,0,0,0,0,0,0},
			.SizeBits = 8
		};
		instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = immediateOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Immediate };
		instruction.OperandCount = 2;
		return true;
	}

	inline DEFINE_HANDLER(Handle_SUB_AxEaxRax_imm16imm32_0x2D) {
		ZoneScoped;//NOLINT
		std::ignore = prefixes;
		instruction.Type = INSTRUCTIONS::InstructionType::SUB;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;


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


		const auto destinationRegister = INSTRUCTIONS::TargetRegister::RAX;
		const uint8_t destinationFlags = static_cast<uint8_t>(std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
			std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister));
		const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
			.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationRegister)),
			.SizeBits = operandSize,
			.Flags = destinationFlags
		};
		instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		if (operandSize == 16) {
			const auto immediateValue = std::bit_cast<std::array<uint8_t, 2>>(static_cast<uint16_t>(core.FetchBytes(address, 2)));
			address += 2;
			instruction.InstructionLengthBytes += 2;
			const auto immediateOperand = INSTRUCTIONS::OPERANDS::ImmediateOperand{
				.Value = {immediateValue[0],immediateValue[1],0,0,0,0,0,0},
				.SizeBits = 32
			};
			instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = immediateOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Immediate };
		}
		else if (operandSize == 32) {
			const auto immediateValue = std::bit_cast<std::array<uint8_t, 4>>(static_cast<uint32_t>(core.FetchBytes(address, 4)));
			address += 4;
			instruction.InstructionLengthBytes += 4;
			const auto immediateOperand = INSTRUCTIONS::OPERANDS::ImmediateOperand{
				.Value = {immediateValue[0],immediateValue[1],immediateValue[2],immediateValue[3],0,0,0,0},
				.SizeBits = 32
			};
			instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = immediateOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Immediate };
		}
		else if (operandSize == 64) {
			const auto immediateValue = static_cast<uint64_t>(static_cast<int64_t>(core.FetchBytes(address, 4)));
			address += 4;
			instruction.InstructionLengthBytes += 8;//sign extension takes up the rest of the 8 bytes. si its 4 bytes immediate + 4 bytes of sign extension
			const auto immediateOperand = INSTRUCTIONS::OPERANDS::ImmediateOperand{
				.Value = std::bit_cast<std::array<uint8_t,8>>(immediateValue),
				.SizeBits = 64
			};
			instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = immediateOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Immediate };
		}
		else {
			NeverOrAssert(false);
		}
		return true;
	}
	inline DEFINE_HANDLER(Handle_REX_INCr16AXr32_BASE) {
		ZoneScoped;//NOLINT

		std::ignore = address;
		std::ignore = instruction;
		if (core.getMode() == vCoreMode::longMode) {
			prefixes.RexPrefix = std::bit_cast<X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::REX>(byte);
			return false;
		}

		throw EXCEPTIONS::UNDEFINED_OPCODE("Handeling for INCr16AXr32 is not yet implemnted ");
		return true;
	}


	inline DEFINE_HANDLER(Handle_GROUP1_0X83_) {
		ZoneScoped;//NOLINT

		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;

		uint8_t operandSize = core.GetDefaultOperandSize();
		assert(std::bit_cast<uint8_t>(INSTRUCTIONS::REX()) == false);
		static_assert(!std::bit_cast<uint8_t>(INSTRUCTIONS::REX().reserved));//reserved is guaranteed to be 0100 for a present REX prefix (true)
		 INSTRUCTIONS::OPERANDS::ImmediateOperand sourceOperand{
			.Value = {0,0,0,0,0,0,0,0},//will be set later
			.SizeBits = 0,//will be set later
		};
		DigestModrmSib(ModrmSib, hasSIB);
		std::ignore = hasSIB;
		if (std::bit_cast<bool>(prefixes.RexPrefix) && prefixes.RexPrefix.W) {
			operandSize = 64;
			sourceOperand.SizeBits = 64;
			const auto immediateValue = static_cast<uint64_t>(static_cast<int64_t>(core.FetchBytes(address, 1)));
			sourceOperand.Value = std::bit_cast<std::array<uint8_t, 8>>(immediateValue);
			sourceOperand.SizeBits = 64;
			instruction.InstructionLengthBytes += 1;
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
				const std::array<uint8_t, 4> immediateValue = std::bit_cast<std::array<uint8_t, 4>>(static_cast<int32_t>(core.FetchBytes(address, 1)));
				sourceOperand.Value[0] = immediateValue[0];
				sourceOperand.Value[1] = immediateValue[1];
				sourceOperand.Value[2] = immediateValue[2];
				sourceOperand.Value[3] = immediateValue[3];
				sourceOperand.SizeBits = 32;
				instruction.InstructionLengthBytes += 1;
			}
			else if (operandSize == 16) {
				const std::array<uint8_t, 2> immediateValue = std::bit_cast<std::array<uint8_t, 2>>(static_cast<int16_t>(core.FetchBytes(address, 1)));
				sourceOperand.Value[0] = immediateValue[0];
				sourceOperand.Value[1] = immediateValue[1];
				sourceOperand.SizeBits = 16;
				instruction.InstructionLengthBytes += 1;
			}
			else {
				NeverOrAssert(false);
			}
		}

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
			throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands not supported yet for GROUP1 0x83 decoding");
		}

		switch(ModrmSib.first.reg){
			case 0:instruction.Type = INSTRUCTIONS::InstructionType::ADD;break;
			case 1:instruction.Type = INSTRUCTIONS::InstructionType::OR; break;
			case 2:
			case 3:
			case 4:goto fail;
			case 5: instruction.Type = INSTRUCTIONS::InstructionType::SUB; break;
			case 6:
			case 7:
			default: {
			fail:
				std::stringstream msg;
				const auto asIntegerType = static_cast<uint16_t>(instruction.OpcodeBytes[0]);
				msg << "\n\n #UD exception \n \n byte: 0x" << std::hex << asIntegerType
					<< " with ModRM.reg: " << std::dec << static_cast<uint16_t> (ModrmSib.first.reg)
					<< " at RIP: 0x" << std::hex << address << " corresponds to no valid opcode " << std::dec;
				throw EXCEPTIONS::UNDEFINED_OPCODE(msg.str());
			}
		}
		instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = sourceOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Immediate };
		instruction.OperandCount = 2;
		return true;
	}
	


}// namespace X86_64_EMU_SOFT::SYSTEM::CPU