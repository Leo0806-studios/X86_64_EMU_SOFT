#pragma once
#include <cstdint>
#include <tuple>
#include <utility>
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_ENGINE.h"
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/INSTRUCTIONS/OPCODE_BYTES.h"
#include "SYSTEM/CPU/VCORE.h"
#include "SYSTEM/MEMORY/MEMORY.h"

namespace X86_64_EMU_SOFT::SYSTEM::CPU {
	inline bool Handle_MOVrm16rm32r16r32(const VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, uint8_t byte) {
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;
		instruction.Type = INSTRUCTIONS::InstructionType::MOV;
		DecodingEngine::digestModRMAndSIB(address, *core.GetMemoryBus(), instruction);
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
	inline bool Handle_MOVr16r32imm16imm32_BASE(const VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, uint8_t byte) {
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;

		instruction.Type = INSTRUCTIONS::InstructionType::MOV;
		instruction.DestinationRegister = DecodingEngine::GetTargetRegisterfromAdditiveID(static_cast<uint8_t>(byte & static_cast<uint8_t>(0b111)));
		if ((core.getMode() == vCoreMode::realMode && !instruction.OperandOverride) ||
			(core.getMode() == vCoreMode::protectedMode && instruction.OperandOverride)) {
			instruction.DestinationSize = instruction.SourceSize = 16;
			instruction.ImmediateSizeBytes = 2;
		}
		else if ((core.getMode() == vCoreMode::realMode && instruction.OperandOverride) ||
				 (core.getMode() == vCoreMode::protectedMode && !instruction.OperandOverride)) {
			instruction.DestinationSize = instruction.SourceSize = 32;
			instruction.ImmediateSizeBytes = 4;


		}
		for (uint8_t i = 0; i < instruction.ImmediateSizeBytes; i++) {
			instruction.ImmediateBytes[i] = (*core.GetMemoryBus()).Read8(address);
			address++;
			instruction.InstructionLengthBytes++;
		}
		address--;//readjust adress down by one
		return true;
	}

	inline bool Handle_NOP(const VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, uint8_t byte) {//NOSONAR

		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;

		std::ignore = core;
		std::ignore = address;
		
		instruction.Type = INSTRUCTIONS::InstructionType::NOP;

		return true;
	}
	inline bool Handle_UD(const VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, uint8_t byte) {//NOSONAR
		instruction.OpcodeBytes[0] = byte;
		instruction.OpcodeSizeBytes++;
		instruction.InstructionLengthBytes++;

		std::ignore = core;
		std::ignore = address;
		
		instruction.Type = INSTRUCTIONS::InstructionType::UD;
		return true;
	}


}//namespace X86_64_EMU_SOFT::SYSTEM::CPU