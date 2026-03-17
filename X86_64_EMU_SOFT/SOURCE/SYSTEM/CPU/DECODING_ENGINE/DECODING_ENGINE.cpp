#include <tuple>
#include <array>
#include <bit>
#include <cstdint>
#include <sstream>
#include <ios>
#include <utility>
#include <exception>
#include <print>
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_ENGINE.h"
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_HANDLERS/PREFIX_HANDLERS.h"
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_HANDLERS/HANDLERS_SPECIAL.h"
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_HANDLERS/HANDLERS_ALU.h"
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/INSTRUCTIONS/OPCODE_BYTES.h"
#include "SYSTEM/CPU/VCORE.h"
#include "SYSTEM/MEMORY/MEMORY.h"
#include "SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU {
	[[noreturn]]inline static bool ThrowUndefinedOpcode(const VirtualCore& core,  uint64_t& address, INSTRUCTIONS::Instruction& instruction, uint8_t byte) {//NOSONAR
		std::ignore = core; 
		std::ignore = instruction;
		std::stringstream msg;
		const auto asIntegerType = static_cast<uint16_t>(byte);
		msg << "\n\n #UD exception \n \n byte: 0x" << std::hex << asIntegerType << " at RIP: 0x" << std::hex << address << " corresponds to no valid opcode " << std::dec;
		throw EXCEPTIONS::UNDEFINED_OPCODE(msg.str());
	}
	std::array<DecodingEngine::HandlerFunc, 256> DecodingEngine::HandlerFuncs;
	const bool DecodingEngine::HandlerFuncSetupDone=[](){//NOSONAR //NOLINT(cert-err58-cpp,clang-diagnostic-global-constructors)
		try {

		for (auto& func : HandlerFuncs) {
			func = ThrowUndefinedOpcode;
		}
		HandlerFuncs[0xF0] =HandlePrefix; //LOCK
		HandlerFuncs[0xF2] =HandlePrefix; //REPNE/REPNZ
		HandlerFuncs[0xF3] =HandlePrefix; //REP/REPE/REPZ
		HandlerFuncs[0x2E] =HandlePrefix; //CS segment override
		HandlerFuncs[0x36] =HandlePrefix; //SS segment override
		HandlerFuncs[0x3E] =HandlePrefix; //DS segment override
		HandlerFuncs[0x26] =HandlePrefix; //ES segment override
		HandlerFuncs[0x64] =HandlePrefix; //FS segment override
		HandlerFuncs[0x65] =HandlePrefix; //GS segment override
		HandlerFuncs[0x66] =HandlePrefix; //Operand-size override
		HandlerFuncs[0x67] =HandlePrefix; //Address-size override

		//ALU
		HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::ADDrm16r16)] = Handle_ADDrm16r16;
		HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::SUBrm16rm32r16r32)] = Handle_SUBrm16rm32r16r32;
		HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::ORrm16rm32r16r32)] = Handle_ORrm16rm32r16r32;
		HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::GROUP1)] = Handle_GROUP1_0X83;


		//SPECIAL
		HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16r32imm16imm32_BASE) + 0U] = Handle_MOVr16r32imm16imm32_BASE;
		HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16r32imm16imm32_BASE) + 1U] = Handle_MOVr16r32imm16imm32_BASE;
		HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16r32imm16imm32_BASE) + 2U] = Handle_MOVr16r32imm16imm32_BASE;
		HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16r32imm16imm32_BASE) + 3U] = Handle_MOVr16r32imm16imm32_BASE;
		HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16r32imm16imm32_BASE) + 4U] = Handle_MOVr16r32imm16imm32_BASE;
		HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16r32imm16imm32_BASE) + 5U] = Handle_MOVr16r32imm16imm32_BASE;
		HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16r32imm16imm32_BASE) + 6U] = Handle_MOVr16r32imm16imm32_BASE;
		HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16r32imm16imm32_BASE) + 7U] = Handle_MOVr16r32imm16imm32_BASE;
		HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::NOP)] = Handle_NOP;
		HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::UD)] = Handle_UD;
		}
		catch (...) {
			std::print("exception got thrown during construction of opcode table\n aborting...");
			std::terminate();
		}


		return true;
		}();







	 INSTRUCTIONS::Instruction DecodingEngine::DecodeInstruction(const VirtualCore& core)
	{

		  
		uint64_t address = core.RIP.GetValue();
		INSTRUCTIONS::Instruction instruction = {};
		const MEMORY::MemoryBus& memBus = *core.memoryBus;

		uint8_t instructionByte = memBus.Read8(address);
		address++;
		while(!HandlerFuncs[instructionByte](core,address,instruction,instructionByte)){
			instructionByte = memBus.Read8(address);
			address++;
			
		}
		return instruction;
	}


















	inline INSTRUCTIONS::TargetRegister DecodingEngine::DecodeRegisterFromModRMRegField(uint8_t regField) noexcept {
		switch (regField) {
			using enum X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::TargetRegister;
			case 0: return RAX;
			case 1: return RCX;
			case 2: return RDX;
			case 3: return RBX;
			case 4: return RSP;
			case 5: return RBP;
			case 6: return RSI;
			case 7: return RDI;
			default: {
				__assume(false);
			}
		}
	}

	inline INSTRUCTIONS::TargetRegister DecodingEngine::DecodeRegisterFromModRMRMField(uint8_t rmField) noexcept {
		switch (rmField) {
			using enum X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::TargetRegister;
			case 0: return RAX;
			case 1: return RCX;
			case 2: return RDX;
			case 3: return RBX;
			case 4: return RSP;
			case 5: return RBP;
			case 6: return RSI;
			case 7: return RDI;
			default: {
				__assume(false);
			}
		}
	}

	inline INSTRUCTIONS::TargetRegister DecodingEngine::GetTargetRegisterfromAdditiveID(uint8_t regSelector) noexcept {
		__assume(regSelector <= 0b111);
		switch (regSelector) {
			using enum X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::TargetRegister;
			case 0:return RAX;
			case 1: return RCX;
			case 2:return RDX;
			case 3:return  RBX;
			case 4:return  RSP;
			case 5:return  RBP;
			case 6:return RSI;
			case 7:return  RDI;
			default: __assume(false);

		}
	}

	 void DecodingEngine::digestModRMAndSIB(uint64_t& address, const MEMORY::MemoryBus& memoryBus, INSTRUCTIONS::Instruction& instruction) noexcept
	{
		const uint8_t modrmByte = memoryBus.Read8(address);
		instruction.ModRM = std::bit_cast<INSTRUCTIONS::ModRM>(modrmByte);
		instruction.InstructionLengthBytes++;

		address++;
		if (instruction.ModRM.mod != 3 && instruction.ModRM.rm == 4) {
			instruction.hasSIB = true;
			const uint8_t sibByte = memoryBus.Read8(address);
			instruction.SIB = std::bit_cast<INSTRUCTIONS::SIB>(sibByte);
			instruction.InstructionLengthBytes++;
			address++;
		}
	}







}// namespace X86_64_EMU_SOFT::SYSTEM::CPU