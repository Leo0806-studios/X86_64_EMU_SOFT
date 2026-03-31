#pragma once
#include <cstdint>
#include <string>
namespace X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS
{
	enum class InstructionType:uint16_t{
		MOV,

		ADD,
		SUB,
		INC,
		DEC,

		OR,

		UD,
		NOP,

	};
	inline std::string InstrucionTypeToString(InstructionType type)noexcept {
		switch (type) {
			case InstructionType::MOV:return"MOV";
			case InstructionType::ADD:return"ADD";
			case InstructionType::SUB:return"SUB";
			case InstructionType::INC:return"INC";
			case InstructionType::DEC:return"DEC";
			case InstructionType::OR:return"OR";
			case InstructionType::NOP:return"NOP";
			case InstructionType::UD:return "UD";
			default:return "Undefined";
		}
	}
	enum class PrimaryOpcodeByteValue :uint8_t {
		ADD_rm8_r8_0x0 = 0x0,//not handled yet
		ADD_rm16rm32rm64_r16r32r64_0x1 = 0x1,//handled partial
		ADD_r8_rm8_0x2 =0x2,
		ADD_r16r32r64_rm16rm32rm64_0x3=0x3,
		ADD_AL_imm8_0x4 = 0x4,
		ADD_AxEaxRax_imm16imm32_0x5 = 0x5,

		OR_rm8_r8_0x8,//handled partial
		OR_rm16rm32_r16r32_0x9 = 0x9,//handled partial
		OR_r8_rm8_0xA,//handled partialy
		OR_r16r32_rm16rm32_0xB,//handled partialy
		OR_AL_imm8_0xC,//handled partialy
		OR_AxEaxRax_imm16imm32_0xD,//handled partialy

		SUB_rm8_r8_0x28 = 0x28,//handled partial
		SUB_rm16rm32_r16r32_0X29 = 0x29,//handled partial
		SUB_r8_rm8_0x2A,//handled partial
		SUB_r16r32_rm16rm32_0x2B,//handled partial
		SUB_AL_imm8_0x2C,//handled partial
		SUB_AxEaxRax_imm16imm32_0x2D,//handled partial

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


		GROUP1_0X83 = 0x83,//handled partial


		MOV_rm16rm32rm64_r16r32r64_0x89 = 0x89,//handled partial


		NOP_0x90 = 0x90,//handled fully


		MOVr16AXr32EAXr64RAXimm16imm32imm64_0xB8 = 0xB8,//handled fully (?)
		MOVr16CXr32ECXr64RCXimm16imm32imm64_0xB9 = 0xB9,//handled fully (?)
		MOVr16DXr32EDXr64RDXimm16imm32imm64_0xBA = 0xBA,//handled fully (?)
		MOVr16BXr32EBXr64RBXimm16imm32imm64_0xBB = 0xBB,//handled fully (?)
		MOVr16SPr32ESPr64RSPimm16imm32imm64_0xBC = 0xBC,//handled fully (?)
		MOVr16BPr32EBPr64RBPimm16imm32imm64_0xBD = 0xBD,//handled fully (?)
		MOVr16SIr32ESIr64RSIimm16imm32imm64_0xBE = 0xBE,//handled fully (?)
		MOVr16DIr32EDIr64RDIimm16imm32imm64_0xBF = 0xBF,//handled fully (?)


		UD_0xD6 = 0xD6,//handled fully
	};
}// namespace X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS
