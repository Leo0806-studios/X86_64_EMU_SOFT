#pragma once
#include <cstdint>
#include <tuple>
#include <utility>
#include <array>
#include <ios>
#include <sstream>
#include <cstring>
#include <bit>

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
		const auto sourceRegister = DecodingEngine::GetTargetRegister8BitFromModRMRegField(core, ModrmSib.first.reg | static_cast<uint8_t>(prefixes.RexPrefix.B << 3ULL));
		const uint8_t sourceFlags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
			(IsHighRegister(sourceRegister) ? std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister) : static_cast<uint8_t>(0U));
		const INSTRUCTIONS::OPERANDS::RegisterOperand SourceOperand =
		{
			.RegisterPointer = std::bit_cast<std::array<uint8_t,8>>(&core.GetRegister(sourceRegister)),
			.SizeBits = 8,
			.Flags = sourceFlags
		};
		instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = SourceOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };


		if (ModrmSib.first.mod == 3) {
			const auto destinationregister = DecodingEngine::GetTargetRegister8BitFromModRMRMField(core, ModrmSib.first.rm | static_cast<uint8_t> (prefixes.RexPrefix.R << 3ULL));
			const uint8_t destinationFlags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
				(IsHighRegister(destinationregister) ? std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister) : static_cast<uint8_t>(0U));
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
	inline DEFINE_HANDLER(Handle_ADD_r8_rm8_0x2) {
		ZoneScoped;//NOLINT
		instruction.Type = INSTRUCTIONS::InstructionType::ADD;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;




		DigestModrmSib(ModrmSib, hasSIB);
		std::ignore = hasSIB;
		assert(std::bit_cast<uint8_t>(INSTRUCTIONS::REX()) == false);

		const auto destinationRegister = DecodingEngine::GetTargetRegister8BitFromModRMRegField(core, ModrmSib.first.reg | static_cast<uint8_t>(prefixes.RexPrefix.B << 3ULL));
		const uint8_t destinationFlags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister) |
			(IsHighRegister(destinationRegister) ? std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isHighByteRegister) : static_cast<uint8_t>(0U));
		const auto destinationOperand = INSTRUCTIONS::OPERANDS::RegisterOperand{
			.RegisterPointer = std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(destinationRegister)),
			.SizeBits = 8,
			.Flags = destinationFlags
		};
		instruction.Operand0 = INSTRUCTIONS::OPERANDS::Operand{ .Data = destinationOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };
		if (ModrmSib.first.mod == 0b11) {

		}
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
		;
		static_assert(std::bit_cast<uint8_t>(INSTRUCTIONS::REX().reserved) == false);//reserved is guaranteed to be 0100 for a present REX prefix (true)
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

		const INSTRUCTIONS::TargetRegister sourceRegister = DecodingEngine::DecodeRegisterFromModRMRegField(ModrmSib.first.reg | static_cast<uint8_t>(prefixes.RexPrefix.B << 3ULL));
		const INSTRUCTIONS::OPERANDS::RegisterOperand SourceOperand =
		{
			.RegisterPointer = std::bit_cast<std::array<uint8_t,8>>(&core.GetRegister(sourceRegister)),
			.SizeBits = operandSize,
			.Flags = std::to_underlying(INSTRUCTIONS::OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)
		};
		instruction.Operand1 = INSTRUCTIONS::OPERANDS::Operand{ .Data = SourceOperand, .Type = INSTRUCTIONS::OPERANDS::OperandType::Register };


		if (ModrmSib.first.mod == 3) {
			const INSTRUCTIONS::TargetRegister destinationregister = DecodingEngine::DecodeRegisterFromModRMRMField(ModrmSib.first.rm | static_cast<uint8_t> (prefixes.RexPrefix.R << 3ULL));
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



	inline bool Handle_OR_rm16rm32_r16r32_0x9(const VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, uint8_t byte) {
		ZoneScoped;//NOLINT
		instruction.Type = INSTRUCTIONS::InstructionType::OR;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;


		const vCoreMode mode = core.getMode();
		DecodingEngine::digestModRMAndSIB(address, core, instruction);
		if ((core.getMode() == vCoreMode::realMode && !instruction.OperandOverride) ||
			(core.getMode() == vCoreMode::protectedMode && instruction.OperandOverride)) {
			instruction.DestinationSize = instruction.SourceSize = 16;
		}
		else if (((mode == vCoreMode::realMode && instruction.OperandOverride) ||
				  (mode == vCoreMode::protectedMode && !instruction.OperandOverride)) || mode == vCoreMode::longMode) {
			instruction.DestinationSize = instruction.SourceSize = 32;

		}
		instruction.SourceRegister = DecodingEngine::DecodeRegisterFromModRMRegField(instruction.ModRM.reg);
		if (instruction.ModRM.mod == 3) {
			instruction.DestinationRegister = DecodingEngine::DecodeRegisterFromModRMRMField(instruction.ModRM.rm);
		}
		return true;
	}





	inline bool Handle_SUB_rm16rm32_r16r32_0X29(const VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, uint8_t byte) {
		ZoneScoped;//NOLINT

		instruction.OpcodeBytes[0] = byte;

		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;


		instruction.Type = INSTRUCTIONS::InstructionType::SUB;
		const vCoreMode mode = core.getMode();
		DecodingEngine::digestModRMAndSIB(address, core, instruction);
		if ((mode == vCoreMode::realMode && !instruction.OperandOverride) ||
			(mode == vCoreMode::protectedMode && instruction.OperandOverride)) {
			instruction.DestinationSize = instruction.SourceSize = 16;
		}
		else if (((mode == vCoreMode::realMode && instruction.OperandOverride) ||
				  (mode == vCoreMode::protectedMode && !instruction.OperandOverride)) || mode == vCoreMode::longMode) {
			instruction.DestinationSize = instruction.SourceSize = 32;

		}
		instruction.SourceRegister = DecodingEngine::DecodeRegisterFromModRMRegField(instruction.ModRM.reg);
		if (instruction.ModRM.mod == 3) {
			instruction.DestinationRegister = DecodingEngine::DecodeRegisterFromModRMRMField(instruction.ModRM.rm);
		}
		return true;
	}

	inline bool Handle_REX_INCr16AXr32_BASE(const VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, uint8_t byte) {
		ZoneScoped;//NOLINT

		std::ignore = address;
		if (core.getMode() == vCoreMode::longMode) {
			instruction.hasREX = true;
			instruction.REX = std::bit_cast<X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::REX>(byte);
			return false;
		}
		if (core.getMode() == vCoreMode::protectedMode || core.getMode() == vCoreMode::realMode) {

			instruction.SourceSize = instruction.DestinationSize = 16;
		}
		else {
			instruction.SourceSize = instruction.DestinationSize = 32;
		}
		instruction.Type = INSTRUCTIONS::InstructionType::INC;
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;
		instruction.DestinationRegister = DecodingEngine::GetTargetRegisterfromAdditiveID(static_cast<uint8_t>(byte & static_cast<uint8_t>(0b111)));
		return true;
	}


	inline bool Handle_GROUP1_0X83(const VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, uint8_t byte) {
		ZoneScoped;//NOLINT

		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;

		const vCoreMode mode = core.getMode();
		DecodingEngine::digestModRMAndSIB(address, core, instruction);
		switch (instruction.ModRM.reg) {
			case 0: {
				instruction.Type = INSTRUCTIONS::InstructionType::ADD;
				if ((mode == vCoreMode::realMode && !instruction.OperandOverride) ||
					(mode == vCoreMode::protectedMode && instruction.OperandOverride)) {
					instruction.DestinationSize = instruction.SourceSize = 16;
					instruction.ImmediateSizeBytes = 2;
					instruction.InstructionLengthBytes++;
					const int16_t signExtend = static_cast<int8_t>(core.FetchBytes(address, 1));//NOLINT(bugprone-signed-char-misuse,cert-str34-c)
					std::array<uint8_t, 2> arr = { 0,0 };
					std::memcpy(arr.data(), &signExtend, arr.size());//NOLINT(clang-diagnostic-unsafe-buffer-usage-in-libc-call)
					instruction.ImmediateBytes[0] = arr[0];
					instruction.ImmediateBytes[1] = arr[1];
					instruction.DestinationRegister = DecodingEngine::DecodeRegisterFromModRMRegField(instruction.ModRM.rm);
				}
				else if (((mode == vCoreMode::realMode && instruction.OperandOverride) ||
						  (mode == vCoreMode::protectedMode && !instruction.OperandOverride)) || mode == vCoreMode::longMode) {
					instruction.DestinationSize = instruction.SourceSize = 32;
					instruction.ImmediateSizeBytes = 4;
					instruction.InstructionLengthBytes++;
					const int32_t signExtend = static_cast<int8_t>(core.FetchBytes(address, 1));//NOLINT(bugprone-signed-char-misuse,cert-str34-c)
					std::array<uint8_t, 4> arr = { 0,0,0,0 };
					std::memcpy(arr.data(), &signExtend, arr.size());//NOLINT(clang-diagnostic-unsafe-buffer-usage-in-libc-call)
					instruction.ImmediateBytes[0] = arr[0];
					instruction.ImmediateBytes[1] = arr[1];
					instruction.ImmediateBytes[2] = arr[2];
					instruction.ImmediateBytes[3] = arr[3];
					instruction.DestinationRegister = DecodingEngine::DecodeRegisterFromModRMRegField(instruction.ModRM.rm);


				}
				break;
			}
			case 1: {
				instruction.Type = INSTRUCTIONS::InstructionType::OR;
				if ((core.getMode() == vCoreMode::realMode && !instruction.OperandOverride) ||
					(core.getMode() == vCoreMode::protectedMode && instruction.OperandOverride)) {
					instruction.DestinationSize = instruction.SourceSize = 16;
					instruction.ImmediateSizeBytes = 2;
					instruction.InstructionLengthBytes++;
					const int16_t signExtend = static_cast<int8_t>(core.FetchBytes(address, 1));//NOLINT(bugprone-signed-char-misuse,cert-str34-c)
					std::array<uint8_t, 2> arr = { 0,0 };
					std::memcpy(arr.data(), &signExtend, arr.size());//NOLINT(clang-diagnostic-unsafe-buffer-usage-in-libc-call)
					instruction.ImmediateBytes[0] = arr[0];
					instruction.ImmediateBytes[1] = arr[1];
					instruction.DestinationRegister = DecodingEngine::DecodeRegisterFromModRMRegField(instruction.ModRM.rm);
				}
				else if ((core.getMode() == vCoreMode::realMode && instruction.OperandOverride) ||
						(core.getMode() == vCoreMode::protectedMode && !instruction.OperandOverride)) {
					instruction.DestinationSize = instruction.SourceSize = 32;
					instruction.ImmediateSizeBytes = 4;
					instruction.InstructionLengthBytes++;
					const int32_t signExtend = static_cast<int8_t>(core.FetchBytes(address, 1));//NOLINT(bugprone-signed-char-misuse,cert-str34-c)
					std::array<uint8_t, 4> arr = { 0,0,0,0 };
					std::memcpy(arr.data(), &signExtend, arr.size());//NOLINT(clang-diagnostic-unsafe-buffer-usage-in-libc-call)
					instruction.ImmediateBytes[0] = arr[0];
					instruction.ImmediateBytes[1] = arr[1];
					instruction.ImmediateBytes[2] = arr[2];
					instruction.ImmediateBytes[3] = arr[3];
					instruction.DestinationRegister = DecodingEngine::DecodeRegisterFromModRMRegField(instruction.ModRM.rm);
				}
				break;
			}
			case 2:
			case 3:
			case 4:goto fail;
			case 5: {
				instruction.Type = INSTRUCTIONS::InstructionType::SUB;
				if ((core.getMode() == vCoreMode::realMode && !instruction.OperandOverride) ||
					(core.getMode() == vCoreMode::protectedMode && instruction.OperandOverride)) {
					instruction.DestinationSize = instruction.SourceSize = 16;
					instruction.ImmediateSizeBytes = 2;
					instruction.InstructionLengthBytes++;
					const int16_t signExtend = static_cast<int8_t>(core.FetchBytes(address, 1));//NOLINT(bugprone-signed-char-misuse,cert-str34-c)
					std::array<uint8_t, 2> arr = { 0,0 };
					std::memcpy(arr.data(), &signExtend, arr.size());//NOLINT(clang-diagnostic-unsafe-buffer-usage-in-libc-call)
					instruction.ImmediateBytes[0] = arr[0];
					instruction.ImmediateBytes[1] = arr[1];
					instruction.DestinationRegister = DecodingEngine::DecodeRegisterFromModRMRegField(instruction.ModRM.rm);
				}
				else if ((core.getMode() == vCoreMode::realMode && instruction.OperandOverride) ||
						(core.getMode() == vCoreMode::protectedMode && !instruction.OperandOverride)) {
					instruction.DestinationSize = instruction.SourceSize = 32;
					instruction.ImmediateSizeBytes = 4;
					instruction.InstructionLengthBytes++;
					const int32_t signExtend = static_cast<int8_t>(core.FetchBytes(address, 1));//NOLINT(bugprone-signed-char-misuse,cert-str34-c)
					std::array<uint8_t, 4> arr = { 0,0,0,0 };
					std::memcpy(arr.data(), &signExtend, arr.size());//NOLINT(clang-diagnostic-unsafe-buffer-usage-in-libc-call)
					instruction.ImmediateBytes[0] = arr[0];
					instruction.ImmediateBytes[1] = arr[1];
					instruction.ImmediateBytes[2] = arr[2];
					instruction.ImmediateBytes[3] = arr[3];
					instruction.DestinationRegister = DecodingEngine::DecodeRegisterFromModRMRegField(instruction.ModRM.rm);


				}
				break;
			}
			case 6:
			case 7:
			default: {
			fail:
				std::stringstream msg;
				const auto asIntegerType = static_cast<uint16_t>(instruction.OpcodeBytes[0]);
				msg << "\n\n #UD exception \n \n byte: 0x" << std::hex << asIntegerType
					<< " with ModRM.reg: " << std::dec << static_cast<uint16_t> (instruction.ModRM.reg)
					<< " at RIP: 0x" << std::hex << address << " corresponds to no valid opcode " << std::dec;
				throw EXCEPTIONS::UNDEFINED_OPCODE(msg.str());
			}
		}
		return true;
	}


}// namespace X86_64_EMU_SOFT::SYSTEM::CPU