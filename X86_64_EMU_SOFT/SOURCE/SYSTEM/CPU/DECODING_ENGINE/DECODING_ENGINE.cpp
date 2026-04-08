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
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_ENGINE.h"
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_HANDLERS/HANDLERS_ALU.h"
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_HANDLERS/HANDLERS_SPECIAL.h"
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_HANDLERS/PREFIX_HANDLERS.h"
#include "SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h"
#include "SYSTEM/CPU/INSTRUCTIONS/OPCODE_BYTES.h"
#include "SYSTEM/CPU/VCORE.h"

#include "SYSTEM/MEMORY/MEMORY.h"

namespace X86_64_EMU_SOFT::SYSTEM::CPU {
	[[noreturn]] inline static bool ThrowUndefinedOpcode(VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, INSTRUCTIONS::Prefixes& prefixes, uint8_t byte) {//NOSONAR
		DeepZoneScoped;//NOLINT
		std::ignore = core;
		std::ignore = instruction;
		std::ignore = prefixes;
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
			HandlerFuncs[0xF0] = Handle_Prefix; //LOCK
			HandlerFuncs[0xF2] = Handle_Prefix; //REPNE/REPNZ
			HandlerFuncs[0xF3] = Handle_Prefix; //REP/REPE/REPZ
			HandlerFuncs[0x2E] = Handle_Prefix; //CS segment override
			HandlerFuncs[0x36] = Handle_Prefix; //SS segment override
			HandlerFuncs[0x3E] = Handle_Prefix; //DS segment override
			HandlerFuncs[0x26] = Handle_Prefix; //ES segment override
			HandlerFuncs[0x64] = Handle_Prefix; //FS segment override
			HandlerFuncs[0x65] = Handle_Prefix; //GS segment override
			HandlerFuncs[0x66] = Handle_Prefix; //Operand-size override
			HandlerFuncs[0x67] = Handle_Prefix; //Address-size override

			//ALU
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::ADD_rm8_r8_0x0)] = Handle_ADD_rm8_r8_0x0;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::ADD_rm16rm32rm64_r16r32r64_0x1)] = Handle_ADD_rm16rm32rm64_r16r32r64_0x1;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::ADD_r8_rm8_0x2)] = Handle_ADD_r8_rm8_0x2;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::ADD_r16r32r64_rm16rm32rm64_0x3)] = Handle_ADD_r16r32r64_rm16rm32rm64_0x3;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::ADD_AL_imm8_0x4)] = Handle_ADD_AL_imm8_0x4;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::ADD_AxEaxRax_imm16imm32_0x5)] = Handle_ADD_AxEaxRax_imm16imm32_0x5;
			
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::OR_rm8_r8_0x8)] = Handle_OR_rm8_r8_0x8;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::OR_rm16rm32_r16r32_0x9)] = Handle_OR_rm16rm32rm64_r16r32r64_0x9;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::OR_r8_rm8_0xA)] = Handle_OR_r8_rm8_0xA;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::OR_r16r32_rm16rm32_0xB)] = Handle_OR_r16r32r64_rm16rm32rm64_0xB;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::OR_AL_imm8_0xC)] = Handle_OR_AL_imm8_0xC;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::OR_AxEaxRax_imm16imm32_0xD)] = Handle_OR_AxEaxRax_imm16imm32_0xD;

			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::SUB_rm8_r8_0x28)] = Handle_SUB_rm8_r8_0x28;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::SUB_rm16rm32_r16r32_0X29)] = Handle_SUB_rm16rm32rm64_r16r32r64_0x29;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::SUB_r8_rm8_0x2A)] = Handle_SUB_r8_rm8_0x2A;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::SUB_r16r32_rm16rm32_0x2B)] = Handle_SUB_r16r32r64_rm16rm32rm64_0x2B;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::SUB_AL_imm8_0x2C)] = Handle_SUB_AL_imm8_0x2C;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::SUB_AxEaxRax_imm16imm32_0x2D)] = Handle_SUB_AxEaxRax_imm16imm32_0x2D;

			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::GROUP1_0X83)] = Handle_GROUP1_0X83_;

			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16AXr32EAX)] = Handle_REX_INCr16AXr32_BASE;//handled partialy
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16CXr32ECX)] = Handle_REX_INCr16AXr32_BASE;//handled partialy
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16DXr32EDX)] = Handle_REX_INCr16AXr32_BASE;//handled partialy
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16BXr32EBX)] = Handle_REX_INCr16AXr32_BASE;//handled partialy
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16SPr32ESP)] = Handle_REX_INCr16AXr32_BASE;//handled partialy
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16BPr32EBP)] = Handle_REX_INCr16AXr32_BASE;//handled partialy
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16SIr32ESI)] = Handle_REX_INCr16AXr32_BASE;//handled partialy
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16DIr32EDI)] = Handle_REX_INCr16AXr32_BASE;//handled partialy


			//SPECIAL
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16AXr32EAXr64RAXimm16imm32imm64_0xB8 )] = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16CXr32ECXr64RCXimm16imm32imm64_0xB9 )] = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16DXr32EDXr64RDXimm16imm32imm64_0xBA )] = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16BXr32EBXr64RBXimm16imm32imm64_0xBB )] = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16SPr32ESPr64RSPimm16imm32imm64_0xBC )] = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16BPr32EBPr64RBPimm16imm32imm64_0xBD )] = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16SIr32ESIr64RSIimm16imm32imm64_0xBE )] = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16DIr32EDIr64RDIimm16imm32imm64_0xBF )] = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOV_rm16rm32rm64_r16r32r64_0x89)] = Handle_MOV_rm16rm32rm64_r16r32r64_0x89;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::NOP_0x90)] = Handle_NOP_0x90;
			HandlerFuncs[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::UD_0xD6)] = Handle_UD_0xD6;
		}
		catch (...) {
			std::print("exception got thrown during construction of opcode table\n aborting...");
			std::terminate();
		}


		return true;
		}();























	INSTRUCTIONS::TargetRegister DecodingEngine::DecodeTargetRegister(uint8_t regSelector) noexcept
	{
		DeepZoneScoped;//NOLINT
		switch (regSelector) {
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
		__assume(false);
	}

	inline INSTRUCTIONS::TargetRegister DecodingEngine::DecodeRegisterFromModRMRegField(uint8_t regField) noexcept {
		DeepZoneScoped;//NOLINT
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
		__assume(false);
	}

	inline INSTRUCTIONS::TargetRegister DecodingEngine::DecodeRegisterFromModRMRMField(uint8_t rmField) noexcept {
		DeepZoneScoped;//NOLINT
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
		__assume(false);
	}

	inline INSTRUCTIONS::TargetRegister DecodingEngine::GetTargetRegisterfromAdditiveID(uint8_t regSelector) noexcept {
		DeepZoneScoped;//NOLINT
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
		__assume(false);
	}

	INSTRUCTIONS::TargetRegister DecodingEngine::DecodeTarget8BitRegister(vCoreMode cpuMode,uint8_t regSelector, bool hasRex) noexcept
	{
		DeepZoneScoped;//NOLINT


		switch(regSelector) {
			using enum X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::TargetRegister;
			case 0: return RAX;
			case 1: return RCX;
			case 2: return RDX;
			case 3: return RBX;
			case 4: {
				if (hasRex&&(cpuMode == vCoreMode::longMode)) {
					return RSP;
				}
				else {
					return AH;
				}
			}
			case 5: {
				if (hasRex && (cpuMode == vCoreMode::longMode)) {
					return RBP;
				}
				else {
					return CH;
				}
			}
			case 6: {
				if (hasRex && (cpuMode == vCoreMode::longMode)) {
					return RSI;
				}
				else {
					return DH;
				}
			}
			case 7: {
				if (hasRex && (cpuMode == vCoreMode::longMode)) {
					return RDI;
				}
				else {
					return BH;
				}
			}
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

		__assume(false);

	}

	INSTRUCTIONS::TargetRegister DecodingEngine::GetTargetRegister8BitFromModRMRegField(const VirtualCore& core, uint8_t  regField) noexcept
	{
		DeepZoneScoped;//NOLINT
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
		__assume(false);

	}

	INSTRUCTIONS::TargetRegister DecodingEngine::GetTargetRegister8BitFromModRMRMField(const VirtualCore& core, uint8_t regField) noexcept
	{
		DeepZoneScoped;//NOLINT
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
		__assume(false);
	}

	INSTRUCTIONS::TargetRegister DecodingEngine::GetFullRegisterFromHighRegister(INSTRUCTIONS::TargetRegister reg) noexcept
	{
		DeepZoneScoped;//NOLINT
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
			case CR0:
			case MSR_EFER:
			case None:
			default: NeverOrAssert(false);
		}
		__assume(false);
	}

	bool DecodingEngine::digestModRMAndSIB(uint64_t& address, const  VirtualCore& core,INSTRUCTIONS::Instruction& instruction ,std::pair<INSTRUCTIONS::ModRM, INSTRUCTIONS::SIB>& ModrmSib) noexcept
	{
		DeepZoneScoped;//NOLINT
		const auto modrmByte = static_cast<uint8_t>(core.FetchBytes(address, 1));
		ModrmSib.first = std::bit_cast<INSTRUCTIONS::ModRM>(modrmByte);
		instruction.InstructionLengthBytes++;

		address++;
		if (ModrmSib.first.mod != 3 && ModrmSib.first.rm == 4) {
			const auto sibByte = static_cast<uint8_t>(core.FetchBytes(address, 1));
			ModrmSib.second= std::bit_cast<INSTRUCTIONS::SIB>(sibByte);
			instruction.InstructionLengthBytes++;
			address++;
			return true;
		}
		return false;
	}









}// namespace X86_64_EMU_SOFT::SYSTEM::CPU