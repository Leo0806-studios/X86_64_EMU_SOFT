#include <array>
#include <bit>
#include <cstdint>
#include <exception>
#include <HELPERS/MACROS.h>
#include <ios>
#include <print>
#include <sstream>
#include <tracy/Tracy.hpp>
#include <tuple>
#include <utility>
#include <HELPERS/MACROS.h>
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_ENGINE.h"
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_HANDLERS/HANDLERS_ALU.h"
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_HANDLERS/HANDLERS_SPECIAL.h"
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_HANDLERS/PREFIX_HANDLERS.h"
#include "SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h"
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/INSTRUCTIONS/OPCODE_BYTES.h"
#include "SYSTEM/CPU/VCORE.h"

#include "SYSTEM/MEMORY/MEMORY.h"
#include "HELPERS/REDEFINE_MACROS.h"

namespace X86_64_EMU_SOFT::SYSTEM::CPU {
	[[noreturn]] inline static bool ThrowUndefinedOpcode(const VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, uint8_t byte) {//NOSONAR
		DeepZoneScoped;
		std::ignore = core;
		std::ignore = instruction;
		std::stringstream msg;
		const auto asIntegerType = static_cast<uint16_t>(byte);
		msg << "\n\n #UD exception \n \n byte: 0x" << std::hex << asIntegerType << " at RIP: 0x" << std::hex << address << " corresponds to no valid opcode " << std::dec;
		throw EXCEPTIONS::UNDEFINED_OPCODE(msg.str());
	}
	std::array<DecodingEngine::HandlerFunc, 256> DecodingEngine::HandlerFuncs;
	const bool DecodingEngine::HandlerFuncSetupDone = []() {//NOSONAR //NOLINT(cert-err58-cpp,clang-diagnostic-global-constructors)
		try {

			for (auto& func : HandlerFuncs) {
				func = ThrowUndefinedOpcode;
			}
			HandlerFuncs[0xF0] = HandlePrefix; //LOCK
			HandlerFuncs[0xF2] = HandlePrefix; //REPNE/REPNZ
			HandlerFuncs[0xF3] = HandlePrefix; //REP/REPE/REPZ
			HandlerFuncs[0x2E] = HandlePrefix; //CS segment override
			HandlerFuncs[0x36] = HandlePrefix; //SS segment override
			HandlerFuncs[0x3E] = HandlePrefix; //DS segment override
			HandlerFuncs[0x26] = HandlePrefix; //ES segment override
			HandlerFuncs[0x64] = HandlePrefix; //FS segment override
			HandlerFuncs[0x65] = HandlePrefix; //GS segment override
			HandlerFuncs[0x66] = HandlePrefix; //Operand-size override
			HandlerFuncs[0x67] = HandlePrefix; //Address-size override

			//ALU
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::ADD_rm16rm32rm64_r16r32r64_0x1)] = Handle_ADD_rm16rm32rm64_r16r32r64_0x1;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::SUBrm16rm32r16r32)] = Handle_SUBrm16rm32r16r32;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::ORrm16rm32r16r32)] = Handle_ORrm16rm32r16r32;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::GROUP1)] = Handle_GROUP1_0X83;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16AXr32EAX)] = Handle_REX_INCr16AXr32_BASE;//handled fully
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16CXr32ECX)] = Handle_REX_INCr16AXr32_BASE;//handled fully
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16DXr32EDX)] = Handle_REX_INCr16AXr32_BASE;//handled fully
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16BXr32EBX)] = Handle_REX_INCr16AXr32_BASE;//handled fully
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16SPr32ESP)] = Handle_REX_INCr16AXr32_BASE;//handled fully
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16BPr32EBP)] = Handle_REX_INCr16AXr32_BASE;//handled fully
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16SIr32ESI)] = Handle_REX_INCr16AXr32_BASE;//handled fully
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16DIr32EDI)] = Handle_REX_INCr16AXr32_BASE;//handled fully


			//SPECIAL
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16AXr32EAXimm16imm32)] = Handle_MOVr16r32imm16imm32_BASE;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16CXr32ECXimm16imm32)] = Handle_MOVr16r32imm16imm32_BASE;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16DXr32EDXimm16imm32)] = Handle_MOVr16r32imm16imm32_BASE;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16BXr32EBXimm16imm32)] = Handle_MOVr16r32imm16imm32_BASE;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16SPr32ESPimm16imm32)] = Handle_MOVr16r32imm16imm32_BASE;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16BPr32EBPimm16imm32)] = Handle_MOVr16r32imm16imm32_BASE;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16SIr32ESIimm16imm32)] = Handle_MOVr16r32imm16imm32_BASE;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16DIr32EDIimm16imm32)] = Handle_MOVr16r32imm16imm32_BASE;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVrm16rm32r16r32)] = Handle_MOVrm16rm32r16r32;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::NOP)] = Handle_NOP;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::UD)] = Handle_UD;
		}
		catch (...) {
			std::print("exception got thrown during construction of opcode table\n aborting...");
			std::terminate();
		}


		return true;
		}();























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
			case 8: return R8;
			case 9: return R9;
			case 10: return R10;
			case 11: return R11;
			case 12: return R12;
			case 13: return R13;
			case 14: return R14;
			case 15: return R15;
			default: {
				NeverOrAssert(false);
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
			case 8: return R8;
			case 9: return R9;
			case 10: return R10;
			case 11: return R11;
			case 12: return R12;
			case 13: return R13;
			case 14: return R14;
			case 15: return R15;
			default: {
				NeverOrAssert(false);
			}
		}
	}

	inline INSTRUCTIONS::TargetRegister DecodingEngine::GetTargetRegisterfromAdditiveID(uint8_t regSelector) noexcept {
		__assume(regSelector <= 0b1111);
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
			case 8: return R8;
			case 9: return R9;
			case 10: return R10;
			case 11: return R11;
			case 12: return R12;
			case 13: return R13;
			case 14: return R14;
			case 15: return R15;
			default: NeverOrAssert(false);

		}
	}

	INSTRUCTIONS::TargetRegister DecodingEngine::GetTargetRegister8BitFromModRMRegField(const VirtualCore& core, uint8_t  regField) noexcept
	{
		if (core.getMode() == vCoreMode::longMode) {
			switch (regField) {
				using enum X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::TargetRegister;
				case 0: return RAX;
				case 1: return RCX;
				case 2: return RDX;
				case 3: return RBX;
				case 4: return AH;
				case 5: return CH;
				case 6: return DH;
				case 7: return BH;
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
				default: NeverOrAssert(false);
			}
		}
		else {
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
				default: NeverOrAssert(false);
			}

		}
		return INSTRUCTIONS::TargetRegister();
	}

	INSTRUCTIONS::TargetRegister DecodingEngine::GetFullRegisterFromHighRegister(INSTRUCTIONS::TargetRegister reg) noexcept
	{
		switch (reg) {
			using enum X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::TargetRegister;
			case AH:return RAX;
			case BH:return RBX;
			case CH:return RCX;
			case DH:return RDX;
			case RAX:
			case RBX:
			case RCX:
			case RDX:
			case RSI:
			case RDI:
			case RSP:
			case RBP:
			case R8:
			case R9:
			case R10:
			case R11:
			case R12:
			case R13:
			case R14:
			case R15:
			default: NeverOrAssert(false);
		}
	}

	void DecodingEngine::digestModRMAndSIB(uint64_t& address, const  VirtualCore& core, INSTRUCTIONS::Instruction& instruction) noexcept
	{
		ZoneNamed(digestModRMAndSIB, true);
		const uint8_t modrmByte = static_cast<uint8_t>(core.FetchBytes(address, 1));
		instruction.ModRM = std::bit_cast<INSTRUCTIONS::ModRM>(modrmByte);
		instruction.InstructionLengthBytes++;

		address++;
		if (instruction.ModRM.mod != 3 && instruction.ModRM.rm == 4) {
			instruction.hasSIB = true;
			const uint8_t sibByte = static_cast<uint8_t>(core.FetchBytes(address, 1));
			instruction.SIB = std::bit_cast<INSTRUCTIONS::SIB>(sibByte);
			instruction.InstructionLengthBytes++;
			address++;
		}
	}









}// namespace X86_64_EMU_SOFT::SYSTEM::CPU