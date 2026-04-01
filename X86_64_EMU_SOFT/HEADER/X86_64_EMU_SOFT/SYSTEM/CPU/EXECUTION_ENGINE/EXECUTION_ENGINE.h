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
#define DEFINE_HANDLER(name) inline void name(VirtualCore& core,const INSTRUCTIONS::Instruction& instruction)


//#define PrintBinaryOp(OpName)\
//std::print("Executing {}, {}, {}" , OpName,\
//core.getSubregisterFromSize(bit_cast<Register*>(destinationOperand.RegisterPointer), 8, true),\
//
//	void foo() {
//		using namespace INSTRUCTIONS::OPERANDS;
//		using namespace std;
//		using namespace REGISTERS
//			print("Executing SUB {}, {}",
//													 core.getSubregisterFromSize(bit_cast<Register*>(destinationOperand.RegisterPointer), 8, true),
//													 instruction.Operand1.Type == OperandType::Register ?
//														core.getSubregisterFromSize(bit_cast<Register*>(get<RegisterOperand>(instruction.Operand1.Data).RegisterPointer), 8, get<RegisterOperand>(instruction.Operand1.Data).Flags & to_underlying(RegisterOperandFlags::isHighByteRegister) ? true : false) :
//													 instruction.Operand1.Type == OperandType::Immediate ? "immediate value" : std::string("[" + std::to_string(std::bit_cast<uint64_t>(std::get<MemoryOperand>(instruction.Operand1.Data).Address)) + "]")
//			);
//	}
}//X86_64_EMU_SOFT::SYSTEM::CPU