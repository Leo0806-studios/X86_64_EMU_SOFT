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

	 INSTRUCTIONS::TargetRegister DecodingEngine::GetTargetRegisterfromAdditiveID(uint8_t regSelector) noexcept {
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