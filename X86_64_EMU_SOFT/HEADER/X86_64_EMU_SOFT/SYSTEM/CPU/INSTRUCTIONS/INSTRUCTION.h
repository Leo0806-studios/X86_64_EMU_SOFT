#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <variant>
#include <string>
#include "OPCODE_BYTES.h"
#include <SYSTEM/CPU/REGISTERS/REGISTER_BASE.h>
#include <bit>

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
		NONE = 0xFF
	};
	enum class  PrefixGroup3 : uint8_t {
		OPERAND_SIZE_OVERRIDE = 0x66,
	};
	enum class PrefixGroup4 : uint8_t {
		ADDRESS_SIZE_OVERRIDE = 0x67
	};

	struct ModRM {//NOLINT(altera-struct-pack-align)
		uint8_t rm : 3 = 0;
		uint8_t reg : 3 = 0;
		uint8_t mod : 2 = 0;
	};
	struct SIB {//NOLINT(altera-struct-pack-align)
		uint8_t base : 3 = 0;
		uint8_t index : 3 = 0;
		uint8_t scale : 2 = 0;
	};

	struct REX {
		uint8_t B : 1 = 0;
		uint8_t X : 1 = 0;
		uint8_t R : 1 = 0;
		uint8_t W : 1 = 0;
		uint8_t reserved : 4 = 0; //reserved, should be 6
	};

	struct Prefixes {
		bool OperandSizeOverride = false;
		bool AddressSizeOverride = false;
		REX RexPrefix{ .B = 0, .X = 0,.R = 0,.W = 0,.reserved = 0 };

	};

	enum class TargetRegister :uint8_t {
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


		CR0,
		//CR2,
		//CR3,
		//CR4,
		//CR8,



		MSR_EFER,

		//the folowing 8 bit registers are listed excplicitly since they arent just masked of lower parts of 64 bit registers (eg i cant just set the operand size to 8)

		AH,
		BH,
		CH,
		DH,
		None = 0xFF
	};
	namespace OPERANDS {
		enum class OperandType : uint8_t {
			Memory,
			Register,
			Immediate,
			None = 0xFF
		};

		enum class RegisterOperandFlags : uint8_t {
			isSegmentRegister = 1U << 0U,
			isControlRegister = 1U << 1U,
			isGeneralPurposeRegister = 1U << 2U,
			isHighByteRegister = 1U << 3U,
			isModelSpecificRegister = 1U << 4U,
			isDebugRegister = 1U << 5U,
		};


		struct RegisterOperand {
		private:

		public:
			std::array<uint8_t, 8> RegisterPointer{ 0,0,0,0,0,0,0,0 };
			uint8_t SizeBits = 0;
			uint8_t Flags = 0;//Bitfield for various flags

		};



		struct MemoryOperand {
			std::array<uint8_t, 8> Address{ 0,0,0,0,0,0,0,0 };
			uint8_t SizeBits = 0;
		};
		struct ImmediateOperand {
			std::array<uint8_t, 8> Value{ 0,0,0,0,0,0,0,0 };
			uint8_t SizeBits = 0;
		};
		struct [[nodiscard("discarding operands can lead to the emulator misbehaving")]] Operand {
			std::variant<RegisterOperand, MemoryOperand, ImmediateOperand> Data;
			OperandType Type = OperandType::None;

		};
	}// namespace OPERANDS
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
			case TargetRegister::CR0: return "CR0";
			case TargetRegister::MSR_EFER: return "EFER";
			case TargetRegister::AH: return "AH";
			case TargetRegister::BH:return "BH";
			case TargetRegister::CH:return "CH";
			case TargetRegister::DH:return "DH";
			case TargetRegister::None:
			default: return "None";
		}
	}


	struct  Instruction {
		uint8_t InstructionLengthBytes = 0;
		PrefixGroup1 Prefix1 = PrefixGroup1::NONE;
		PrefixGroup2 Prefix2 = PrefixGroup2::NONE;
		uint8_t OpcodeSizeBytes = 0;
		std::array<uint8_t, 3> OpcodeBytes{};
		uint8_t OperandCount = 0;
		InstructionType Type = InstructionType::UD;
		//the format of this type is anagolus to intel nasm syntax. with operand0 being the leftmost while writing the instruction and operand3 being the rightmost. for example in "ADD RAX, [RBX+4]", operand0 would be RAX while operand1 would be [RBX+4] and operand2 and operand3 would be unused


		OPERANDS::Operand Operand0{};
		OPERANDS::Operand Operand1{};
		OPERANDS::Operand Operand2{};
		OPERANDS::Operand Operand3{};

	};


}//namespace X86_64_EMU_SOFT::SYSTEM::CPU::INSRUCTIONS
