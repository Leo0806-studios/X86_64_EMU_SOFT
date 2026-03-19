#pragma once
#include <cstdint>
namespace X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS
{
	enum class InstructionType:uint8_t{
		MOV,

		ADD,
		SUB,

		OR,

		UD,
		NOP,

	};
	enum class PrimaryOpcodeByteValue :uint8_t {
		ADDrm16r16 = 0x1,//handled partial
		ORrm16rm32r16r32 = 0x9,//handled partial
		SUBrm16rm32r16r32 = 0x29,//handled partial

		REX_INCr16AXr32EAX = 0x40,//not handled yet
		REX_INCr16CXr32ECX = 0x41,//not handled yet
		REX_INCr16DXr32EDX = 0x42,//not handled yet
		REX_INCr16BXr32EBX = 0x43,//not handled yet
		REX_INCr16SPr32ESP = 0x44,//not handled yet
		REX_INCr16BPr32EBP = 0x45,//not handled yet
		REX_INCr16SIr32ESI = 0x46,//not handled yet
		REX_INCr16DIr32EDI = 0x47,//not handled yet
		REX_DECr16AXr32EAX = 0x48,//not handled yet
		REX_DECr16CXr32ECX = 0x49,//not handled yet
		REX_DECr16DXr32EDX = 0x4A,//not handled yet
		REX_DECr16BXr32EBX = 0x4B,//not handled yet
		REX_DECr16SPr32ESP = 0x4C,//not handled yet
		REX_DECr16BPr32EBP = 0x4D,//not handled yet
		REX_DECr16SIr32ESI = 0x4E,//not handled yet
		REX_DECr16DIr32EDI = 0x4F,//not handled yet
		GROUP1 = 0x83,//handled partial
		MOVrm16rm32r16r32 = 0x89,//handled partial
		NOP = 0x90,//handled fully


		MOVr16AXr32EAXimm16imm32 = 0xB8,//handled fully (?)
		MOVr16CXr32ECXimm16imm32 = 0xB9,//handled fully (?)
		MOVr16DXr32EDXimm16imm32 = 0xBA,//handled fully (?)
		MOVr16BXr32EBXimm16imm32 = 0xBB,//handled fully (?)
		MOVr16SPr32ESPimm16imm32 = 0xBC,//handled fully (?)
		MOVr16BPr32EBPimm16imm32 = 0xBD,//handled fully (?)
		MOVr16SIr32ESIimm16imm32 = 0xBE,//handled fully (?)
		MOVr16DIr32EDIimm16imm32 = 0xBF,//handled fully (?)


		UD = 0xD6,//handled fully
	};
}// namespace X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS
