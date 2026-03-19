#pragma once
#include <cstdint>
#include <tuple>
#include <utility>
#include <array>
#include <ios>
#include <sstream>
#include <cstring>
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_ENGINE.h"
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/INSTRUCTIONS/OPCODE_BYTES.h"
#include "SYSTEM/CPU/VCORE.h"
#include "SYSTEM/MEMORY/MEMORY.h"
#include "SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h"

namespace X86_64_EMU_SOFT::SYSTEM::CPU {
	inline bool Handle_ADDrm16r16(const VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, uint8_t byte) {
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;


		instruction.SourceSize = 16;
		instruction.DestinationSize = 16;
		instruction.hasModRM = true;
		instruction.Type = INSTRUCTIONS::InstructionType::ADD; 
		DecodingEngine::digestModRMAndSIB(address, core, instruction);
		instruction.SourceRegister = DecodingEngine::DecodeRegisterFromModRMRegField(instruction.ModRM.reg);
		if (instruction.ModRM.mod == 3) {
			instruction.DestinationRegister = DecodingEngine::DecodeRegisterFromModRMRMField(instruction.ModRM.rm);
		}
		return true;
	}



	inline bool Handle_ORrm16rm32r16r32(const VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, uint8_t byte) {
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;


		instruction.Type = INSTRUCTIONS::InstructionType::OR;
		DecodingEngine::digestModRMAndSIB(address, core, instruction);
		if ((core.getMode() == vCoreMode::realMode && !instruction.OperandOverride) ||
			(core.getMode() == vCoreMode::protectedMode && instruction.OperandOverride)) {
			instruction.DestinationSize = instruction.SourceSize = 16;
		}
		else if ((core.getMode() == vCoreMode::realMode && instruction.OperandOverride) ||
				 (core.getMode() == vCoreMode::protectedMode && !instruction.OperandOverride)) {
			instruction.DestinationSize = instruction.SourceSize = 32;

		}
		instruction.SourceRegister = DecodingEngine::DecodeRegisterFromModRMRegField(instruction.ModRM.reg);
		if (instruction.ModRM.mod == 3) {
			instruction.DestinationRegister = DecodingEngine::DecodeRegisterFromModRMRMField(instruction.ModRM.rm);
		}
		return true; 
	}





	inline bool Handle_SUBrm16rm32r16r32(const VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, uint8_t byte) {
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;


		instruction.Type = INSTRUCTIONS::InstructionType::SUB;
		DecodingEngine::digestModRMAndSIB(address, core, instruction);
		if ((core.getMode() == vCoreMode::realMode && !instruction.OperandOverride) || 
			(core.getMode() == vCoreMode::protectedMode && instruction.OperandOverride)) {
			instruction.DestinationSize = instruction.SourceSize = 16;
		}
		else if ((core.getMode() == vCoreMode::realMode && instruction.OperandOverride) ||
				 (core.getMode() == vCoreMode::protectedMode && !instruction.OperandOverride)) {
			instruction.DestinationSize = instruction.SourceSize = 32;

		}
		instruction.SourceRegister = DecodingEngine::DecodeRegisterFromModRMRegField(instruction.ModRM.reg);
		if (instruction.ModRM.mod == 3) {
			instruction.DestinationRegister = DecodingEngine::DecodeRegisterFromModRMRMField(instruction.ModRM.rm);
		}
		return true;
	}
	inline bool Handle_GROUP1_0X83(const VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, uint8_t byte) {
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;


		DecodingEngine::digestModRMAndSIB(address, core, instruction);
		switch (instruction.ModRM.reg) {
			case 0: {
				instruction.Type = INSTRUCTIONS::InstructionType::ADD;
				if ((core.getMode() == vCoreMode::realMode && !instruction.OperandOverride) ||
					(core.getMode() == vCoreMode::protectedMode && instruction.OperandOverride)) {
					instruction.DestinationSize = instruction.SourceSize = 16;
					instruction.ImmediateSizeBytes = 2;
					instruction.InstructionLengthBytes++;
					const int16_t signExtend = static_cast<int8_t>(core.FetchBytes(address,1));//NOLINT(bugprone-signed-char-misuse,cert-str34-c)
					std::array<uint8_t,2> arr = { 0,0 };
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
					std::array<uint8_t,4> arr = { 0,0,0,0 };
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