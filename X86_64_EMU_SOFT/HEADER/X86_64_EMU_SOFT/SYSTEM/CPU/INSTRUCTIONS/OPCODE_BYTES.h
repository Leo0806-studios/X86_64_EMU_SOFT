#pragma once
#include <cstdint>
namespace X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS
{
	enum class InstructionType{
		MOV,

		ADD,
		SUB,

		OR,

		UD,
		NOP,

	};
	enum class PrimaryOpcodeByteValue :uint8_t {
		ADDrm16r16 = 0x1,
		ORrm16rm32r16r32 = 0x9,
		SUBrm16rm32r16r32 = 0x29,
		GROUP1 = 0x83,
		NOP = 0x90,


		MOVr16r32imm16imm32_BASE = 0xB8,


		UD = 0xD6,
	};
}