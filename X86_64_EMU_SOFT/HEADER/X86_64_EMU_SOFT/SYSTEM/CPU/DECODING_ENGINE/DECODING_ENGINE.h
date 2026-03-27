#pragma once
#include <array>
#include <cstdint>
#include <utility>
#include <SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h>
#include <SYSTEM/CPU/VCORE.h>
#include <SYSTEM/MEMORY/MEMORY.h>
namespace X86_64_EMU_SOFT::SYSTEM::CPU {
	class DecodingEngine {
	private:
		/// <summary>
		/// defines the type for a handler function for instructions. has to return false unless the function finishes handeling of the instruction
		/// after execution address has to point to the first unconsumed byte 
		/// core is the Virtual core on wich the instruction gets executed
		/// address is the the address one past the one that returns "byte"
		/// instruction is the current instruction
		/// byte is the byte current byte of the instruction 
		/// </summary>
		using HandlerFunc = bool(*)(VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, INSTRUCTIONS::Prefixes& prefixes, uint8_t byte);
		const static bool HandlerFuncSetupDone;

	public:
		static std::array<HandlerFunc, 256> HandlerFuncs;

		static INSTRUCTIONS::TargetRegister DecodeTargetRegister(uint8_t regSelector)noexcept;

		/// <summary>
		/// decodes the target register from the reg field of MODRM. is also capable of decoding REX extended registers if the bitIndex 3 is set in the regField
		/// </summary>
		/// <param name="regField"></param>
		/// <returns></returns>
		[[deprecated("use the generic DecodeTargetRegister instead")]] static INSTRUCTIONS::TargetRegister DecodeRegisterFromModRMRegField(uint8_t regField)noexcept;

		/// <summary>
		/// decodes the target register from the rm field of MODRM. is also capable of decoding REX extended registers if the bitIndex 3 is set in the rmField
		/// </summary>
		/// <param name="rmField"></param>
		/// <returns></returns>
		[[deprecated("use the generic DecodeTargetRegister instead")]] static INSTRUCTIONS::TargetRegister DecodeRegisterFromModRMRMField(uint8_t rmField)noexcept;

		/// <summary>
		/// decodes the target register from the additive part of the opcode. is also capable of decoding REX extended registers if the bitIndex 3 is set in the rmField
		/// </summary>
		/// <param name="regSelector"></param>
		/// <returns></returns>
		static INSTRUCTIONS::TargetRegister GetTargetRegisterfromAdditiveID(uint8_t regSelector)noexcept;


		[[nodiscard]] static INSTRUCTIONS::TargetRegister DecodeTarget8BitRegister(const VirtualCore& core, uint8_t regSelector)noexcept;

		/// <summary>
		/// decodes the target register from the MODRM Reg field of the instruction. is also capable of decoding REX extended registers if the bitIndex 3 is set in the rmField
		/// </summary>
		/// <param name="core"></param>
		/// <param name="regField"></param>
		/// <returns></returns>
		[[deprecated("use the generic DecodeTarget8BitRegister instead")]] static INSTRUCTIONS::TargetRegister GetTargetRegister8BitFromModRMRegField(const VirtualCore& core, uint8_t  regField)noexcept;

		/// <summary>
		/// decodes the target register from the MODRM Reg field of the instruction. is also capable of decoding REX extended registers if the bitIndex 3 is set in the rmField
		/// </summary>
		/// <param name="core"></param>
		/// <param name="regField"></param>
		/// <returns></returns>
		[[deprecated("use the generic DecodeTarget8BitRegister instead")]] static INSTRUCTIONS::TargetRegister GetTargetRegister8BitFromModRMRMField(const VirtualCore& core, uint8_t  regField)noexcept;

		/// <summary>
		/// gets the full register from the input high register
		/// eg AH -> RAX, BH -> RBX, CH -> RCX, DH -> RDX
		/// </summary>
		/// <param name="reg"></param>
		/// <returns></returns>
		static INSTRUCTIONS::TargetRegister GetFullRegisterFromHighRegister(INSTRUCTIONS::TargetRegister reg)noexcept;







		/// <summary>
		/// always digests the MODRM byte and then conditionally based on the ModRM byte it also digests the SIB byte
		/// </summary>
		/// <param name="address"></param>
		/// <param name="memoryBus"></param>
		/// <param name="instruction"></param>
		/// <returns>true if a SIB byte was consumed</returns>
		[[nodiscard]] static  bool digestModRMAndSIB(uint64_t& address, const  VirtualCore& core, INSTRUCTIONS::Instruction& instruction, std::pair<INSTRUCTIONS::ModRM, INSTRUCTIONS::SIB>& ModrmSib)noexcept;
	};
#define DEFINE_HANDLER(funcName) bool funcName( VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction,INSTRUCTIONS::Prefixes& prefixes, uint8_t byte)
#define DigestModrmSib(PairName,retName) std::pair<INSTRUCTIONS::ModRM, INSTRUCTIONS::SIB> PairName{};const bool retName = DecodingEngine::digestModRMAndSIB(address, core, instruction, PairName)//NOLINT(bugprone-macro-parentheses)
#define IsHighRegister(reg) (reg == INSTRUCTIONS::TargetRegister::AH || reg == INSTRUCTIONS::TargetRegister::BH || reg == INSTRUCTIONS::TargetRegister::CH || reg == INSTRUCTIONS::TargetRegister::DH)


}// namespace 86_64_EMU_SOFT::SYSTEM::CPU



