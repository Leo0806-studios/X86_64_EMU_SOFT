#pragma once
#include <cstdint>
#include <array>
#include <SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h>
#include <SYSTEM/MEMORY/MEMORY.h>
#include <SYSTEM/CPU/VCORE.h>
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
		using HandlerFunc = bool(*)(const VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction,uint8_t byte);
		static std::array<HandlerFunc, 256> HandlerFuncs;
		const static bool HandlerFuncSetupDone;

	public:
		static INSTRUCTIONS::TargetRegister DecodeRegisterFromModRMRegField(uint8_t regField)noexcept;
		static INSTRUCTIONS::TargetRegister DecodeRegisterFromModRMRMField(uint8_t rmField)noexcept;

		static INSTRUCTIONS::TargetRegister GetTargetRegisterfromAdditiveID(uint8_t regSelector)noexcept;







		/// <summary>
		/// always digests the MODRM byte and then conditionally based on the ModRM byte it also digests the SIB byte
		/// </summary>
		/// <param name="address"></param>
		/// <param name="memoryBus"></param>
		/// <param name="instruction"></param>
		static  void digestModRMAndSIB(uint64_t& address,const  VirtualCore& core, INSTRUCTIONS::Instruction& instruction)noexcept;
		[[nodiscard]] static  INSTRUCTIONS::Instruction DecodeInstruction(const VirtualCore& core);
	};
}