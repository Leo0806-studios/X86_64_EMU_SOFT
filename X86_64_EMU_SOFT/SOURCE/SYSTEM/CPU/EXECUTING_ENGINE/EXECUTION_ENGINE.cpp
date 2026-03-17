#include <string.h>
#include <format>
#include <cstdint>
#include <print>
#include "SYSTEM/CPU/EXECUTION_ENGINE/EXECUTION_ENGINE.h"
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/INSTRUCTIONS/OPCODE_BYTES.h"
#include "SYSTEM/CPU/VCORE.h"
#include "SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h"
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_ENGINE.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU{
	 void ExecutionEngine::ExecuteInstruction(VirtualCore& core, const INSTRUCTIONS::Instruction& instruction)
	{
		switch (instruction.Type) {
			using enum INSTRUCTIONS::InstructionType;
			case MOV: {
				if (instruction.ImmediateSizeBytes > 0) {
					std::print("Executing instruction: MOV r/m{} {}, imm{}\n", instruction.DestinationSize, core.getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize), instruction.SourceSize);
					uint64_t val = 0;

					__assume(instruction.ImmediateBytes.data());
					std::memcpy(&val, instruction.ImmediateBytes.data(), sizeof(uint64_t));
					uint64_t tmpval = core.GetRegisterValue(instruction.DestinationRegister);
					__assume(instruction.DestinationSize < 64);
					const uint64_t mask = (1ULL << instruction.DestinationSize) - 1;
					tmpval &= ~mask;
					tmpval |= val;
					std::print("value of destination register {:#X} (signed {}) before execution\n", core.GetRegisterValue(instruction.DestinationRegister),static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister)));
					core.SetRegisterValue(instruction.DestinationRegister, tmpval);
					std::print("value of destination register {:#X} (signed {}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister),static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister)));
				}
				else {
					throw EXCEPTIONS::UNDEFINED_OPCODE("Register to memory or register to register is not yet supported for MOV");
				}
				break;
			}
			case ADD: {

				if (instruction.ImmediateSizeBytes > 0) {
					std::print("Executing instruction: ADD r/m{} {}, imm{} \n",
		   instruction.DestinationSize,
		   core.getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize),
		   instruction.SourceSize);

					uint64_t sourceVal = 0;
					std::memcpy(&sourceVal, instruction.ImmediateBytes.data(), instruction.InstructionLengthBytes);
					const uint64_t destVal = core.GetRegisterValue(instruction.DestinationRegister);
					std::print("value of Destintion register: {:#X} (signed {}), value of immediat {:#X} (signed: {} before execution\n", destVal,static_cast<int64_t>(destVal), sourceVal,static_cast<int64_t>(sourceVal));
					core.SetRegisterValue(instruction.DestinationRegister, destVal + sourceVal);
					std::print("value of Destiontion Register: {:#X} (signed {}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister),static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister)));
					break;
				}


				if (instruction.ModRM.mod != 0b11) {
					throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands are not supported yet for ADD");
				}
				else {
					std::print("Executing instruction: ADD r/m{} {}, r{} {}\n",
							   instruction.DestinationSize,
							   core.getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize),
							   instruction.SourceSize,
							   core.getSubregisterFromSize(instruction.SourceRegister, instruction.SourceSize));
					const uint64_t sourceVal = core.GetRegisterValue(instruction.SourceRegister);
					const uint64_t destVal = core.GetRegisterValue(instruction.DestinationRegister);
					std::print("value of Destintion register: {:#X} (signed: {}), value of Source Register {:#X} (signed: {}) before execution\n", destVal,static_cast<int64_t>(destVal), sourceVal,static_cast<int64_t>(sourceVal));
					core.SetRegisterValue(instruction.DestinationRegister, destVal + sourceVal);
					std::print("value of Destiontion Register: {:#X} (signed: {}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister),static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister)));
				}




				break;

			}
			case SUB: {
				if (instruction.ImmediateSizeBytes > 0) {
					std::print("Executing instruction: SUB r/m{} {}, imm{} \n",
instruction.DestinationSize,
core.getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize),
instruction.SourceSize);

					uint64_t sourceVal = 0;
					std::memcpy(&sourceVal, instruction.ImmediateBytes.data(), instruction.InstructionLengthBytes);
					const uint64_t destVal = core.GetRegisterValue(instruction.DestinationRegister);
					std::print("value of Destintion register: {:#X} (signed {}), value of immediat {:#X} (signed {}) before execution\n", destVal, static_cast<int64_t>(destVal), sourceVal, static_cast<int64_t>(sourceVal));
					core.SetRegisterValue(instruction.DestinationRegister, destVal - sourceVal);
					std::print("value of Destiontion Register: {:#X} (signed {}) after execution\n", core.GetRegisterValue(instruction.DestinationRegister), static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister)));
					break;
				}

				if (instruction.ModRM.mod != 0b11) {
					throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands are not yet supported for SUB");
				}
				else {
					std::print("Executing instruction: SUB r/m{} {}, r{} {}\n",
		   instruction.DestinationSize,
		  core.getSubregisterFromSize(instruction.DestinationRegister, instruction.DestinationSize),
		   instruction.SourceSize,
		  core.getSubregisterFromSize(instruction.SourceRegister, instruction.SourceSize));
					const uint64_t sourceVal = core.GetRegisterValue(instruction.SourceRegister);
					const uint64_t destVal = core.GetRegisterValue(instruction.DestinationRegister);
					std::print("value of Destintion register: {:#X} (signed {}), value of Source Register {:#X} (signed: {}) before execution\n",
							   destVal, static_cast<int64_t>(destVal), sourceVal, static_cast<int64_t>(sourceVal));
					core.SetRegisterValue(instruction.DestinationRegister, destVal - sourceVal);
					std::print("value of Destiontion Register: {:#X} (signed {}) after execution\n",
							   core.GetRegisterValue(instruction.DestinationRegister), static_cast<int64_t>(core.GetRegisterValue(instruction.DestinationRegister)));
				}

				break;
			}

			case OR: {

			}

			case UD: {
				throw EXCEPTIONS::UNDEFINED_OPCODE(std::format("UD instruction executed at RIP: {:#X}", core.RIP.GetValue()));
			}
			case NOP: {
				std::print("Executing instruction NOP\n");
				break;
			}
		}
	}
}