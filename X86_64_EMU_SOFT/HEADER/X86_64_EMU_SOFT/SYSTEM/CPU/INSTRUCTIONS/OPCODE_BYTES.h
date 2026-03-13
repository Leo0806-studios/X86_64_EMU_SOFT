#pragma once
#include <cstdint>
namespace X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS
{
	enum class InstructionType{
		MOV,

		ADD,


		UD,
		NOP,

	};
	enum PrimaryOpcodeByteValue :uint8_t {
		ADDrm16r16 = 0x1,


		NOP = 0x90,


		MOVr16r32imm16imm32_BASE = 0xB8,


		UD = 0xD6,
	};
}