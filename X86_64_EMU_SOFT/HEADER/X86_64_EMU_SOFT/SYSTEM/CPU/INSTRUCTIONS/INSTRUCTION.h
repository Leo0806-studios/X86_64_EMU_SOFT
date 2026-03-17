#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include "OPCODE_BYTES.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS {

	enum class PrefixGroup1 : uint8_t {
		LOCK = 0xF0,
		REPNE_REPNZ = 0xF2,
		REP_REPE_REPZ = 0xF3,
		NONE = 0xFF

	};
	enum  class PrefixGroup2 : uint8_t {
		CS_SEGMENT_OVERRIDE = 0x2E,
		SS_SEGMENT_OVERRIDE = 0x36,
		DS_SEGMENT_OVERRIDE = 0x3E,
		ES_SEGMENT_OVERRIDE = 0x26,
		FS_SEGMENT_OVERRIDE = 0x64,
		GS_SEGMENT_OVERRIDE = 0x65,
		NONE =0xFF
	};
	enum class  PrefixGroup3 : uint8_t {
		OPERAND_SIZE_OVERRIDE = 0x66,
	};
	enum class PrefixGroup4 : uint8_t {
		ADDRESS_SIZE_OVERRIDE = 0x67
	};

	struct ModRM {//NOLINT(altera-struct-pack-align)
		uint8_t rm : 3;
		uint8_t reg : 3;
		uint8_t mod : 2;
	};
	struct SIB {//NOLINT(altera-struct-pack-align)
		uint8_t base : 3;
		uint8_t index : 3;
		uint8_t scale : 2;
	};
	


	enum class TargetRegister:uint8_t{
		RAX,
		RBX,
		RCX,
		RDX,
		RSI,
		RDI,
		RSP,
		RBP,
		R8,
		R9,
		R10,
		R11,
		R12,
		R13,
		R14,
		R15,
		None = 0xFF
	};
	[[nodiscard]] constexpr std::string RegisterToString(TargetRegister reg) {
		switch (reg) {
			case TargetRegister::RAX: return "RAX";
			case TargetRegister::RBX: return "RBX";
			case TargetRegister::RCX: return "RCX";
			case TargetRegister::RDX: return "RDX";
			case TargetRegister::RSI: return "RSI";
			case TargetRegister::RDI: return "RDI";
			case TargetRegister::RSP: return "RSP";
			case TargetRegister::RBP: return "RBP";
			case TargetRegister::R8: return "R8";
			case TargetRegister::R9: return "R9";
			case TargetRegister::R10: return "R10";
			case TargetRegister::R11: return "R11";
			case TargetRegister::R12: return "R12";
			case TargetRegister::R13: return "R13";
			case TargetRegister::R14: return "R14";
			case TargetRegister::R15: return "R15";
			case TargetRegister::None:
			default: return "None";
		}
	}


	struct Instruction {//NOLINT(altera-struct-pack-align)
		uint8_t InstructionLengthBytes=0;


		PrefixGroup1 Prefix1=PrefixGroup1::NONE;
		PrefixGroup2 Prefix2=PrefixGroup2::NONE;
		bool OperandOverride = false;
		bool AddressOverride = false;
		bool hasModRM = false;
		ModRM ModRM{ .rm=0,.reg=0,.mod=0 };
		bool hasSIB = false;
		SIB SIB{ .base=0,.index=0,.scale=0 };
		uint8_t ImmediateSizeBytes = 0;
		uint8_t OpcodeSizeBytes=0;
		uint8_t DisplacementSizeBytes = 0;
		uint8_t SourceSize=0;
		uint8_t DestinationSize=0;
		TargetRegister SourceRegister=TargetRegister::None;
		TargetRegister DestinationRegister=TargetRegister::None;
		InstructionType Type = InstructionType::UD;
		std::array<uint8_t, sizeof(uint64_t)> ImmediateBytes{};
		std::array<uint8_t, sizeof(uint64_t)> DisplacementBytes{};
		std::array<uint8_t, 3> OpcodeBytes{};

	};


}//namespace X86_64_EMU_SOFT::SYSTEM::CPU::INSRUCTIONS
