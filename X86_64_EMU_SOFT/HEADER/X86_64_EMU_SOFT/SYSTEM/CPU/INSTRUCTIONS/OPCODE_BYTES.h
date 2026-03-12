#pragma once
#include <cstdint>
namespace X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS
{
	enum class InstructionType{
		ADD,
		SUB,
		MUL,
		DIV,
		MOD,
		OR ,
		AND,
		NOT,
		SHR,
		SHL,
	};
}