#pragma once
#include <array>
#include <unordered_map>
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h" 
#include "SYSTEM/CPU/INSTRUCTIONS/OPCODE_BYTES.h"
#include "SYSTEM/CPU/VCORE.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU {

	class ExecutionEngine {
		using HandlerFunc = void(*)(VirtualCore& core, const  INSTRUCTIONS::Instruction& instruction);
		static const bool HandlerFuncsSetupDone;
	public:
		static std::array<HandlerFunc, 256> HandlerFuncs;
	};
//#define DEFINE_HANDLER(name) inline void name(VirtualCore& core,const INSTRUCTIONS::Instruction& instruction)


#define PrintRegister(beforeText,Reg,middleText,sizeBits,highByte,value,castTypeSigned) std::print("{} {} {} {:#X}, {:#B}, {}, signed {}\n",beforeText, core.getSubregisterFromSize(&(Reg), (sizeBits), (highByte)),middleText, (value), (value), (value), static_cast<castTypeSigned>((value)));
#define PrintImmediate(beforeText, value, sizeBits) std::print("{} Immediate value: {:#X}, {:#B}, size: {} bits\n", beforeText, (value), (value), (sizeBits));
}//X86_64_EMU_SOFT::SYSTEM::CPU