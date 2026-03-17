#pragma once
#include <cstdint>
#include <atomic>
#include <memory>
#include <unordered_map>
#include <string>
#include "SYSTEM/CPU/REGISTERS/GPR.h"
#include "SYSTEM/CPU/REGISTERS/CONTROLL_REGISTERS/CR0.h"
#include "SYSTEM/CPU/REGISTERS/MSR/EFER.h"
#include "SYSTEM/CPU/REGISTERS/RIP.h"
#include "SYSTEM/MEMORY/MEMORY.h"
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU
{


	enum class vCoreMode :uint8_t {
		realMode,
		protectedMode,
		longMode,
	};
	inline vCoreMode IntToMode(int modeBits) {
		switch (modeBits) {
			case 16: return vCoreMode::realMode;
			case 32: return vCoreMode::protectedMode;
			case 64: return vCoreMode::longMode;
			default:__assume(false);
		}
	}
	class VirtualCore//NOSONAR
	{
		friend class DecodingEngine;
		friend class ExecutionEngine;
		enum class RegisterID :uint8_t {
			RAX = 0b000,
			RCX = 0b001,
			RDX = 0b010,
			RBX = 0b011,
			RSP = 0b100,
			RBP = 0b101,
			RSI = 0b110,
			RDI = 0b111,
		};
		REGISTERS::GPR RAX;
		REGISTERS::GPR RBX;
		REGISTERS::GPR RCX;
		REGISTERS::GPR RDX;
		REGISTERS::GPR RSI;
		REGISTERS::GPR RDI;
		REGISTERS::GPR RSP;
		REGISTERS::GPR RBP;
		REGISTERS::GPR R8;
		REGISTERS::GPR R9;
		REGISTERS::GPR R10;
		REGISTERS::GPR R11;
		REGISTERS::GPR R12;
		REGISTERS::GPR R13;
		REGISTERS::GPR R14;
		REGISTERS::GPR R15;

		REGISTERS::RIP RIP;

		REGISTERS::EFER EFER;
		REGISTERS::CR0 CR0;



		std::atomic_bool isRunning;
		std::atomic_bool hasShutdown;
		std::atomic_bool isEnabled;
		static const uint64_t EFER_MSR_NUMBER = 0xC0000080ULL;

		std::shared_ptr<MEMORY::MemoryBus> memoryBus;
		[[nodiscard]] INSTRUCTIONS::Instruction decodeInstruction() const;
		void executeInstruction(const INSTRUCTIONS::Instruction& instruction);
		void PrintCoreState()const;
	public:
		explicit VirtualCore(uint64_t resetVector, std::shared_ptr<MEMORY::MemoryBus> memoryBus, vCoreMode startupMode)noexcept;
		VirtualCore(const VirtualCore& other)noexcept;
		VirtualCore& operator=(const VirtualCore& other)noexcept;
		VirtualCore(VirtualCore&& other)noexcept;
		VirtualCore& operator=(VirtualCore&& other)noexcept;
		~VirtualCore();
		[[nodiscard]] bool isCoreRunning() const noexcept {
			return isRunning.load();
		}
		[[nodiscard]]
		bool hasShutdownCore() const noexcept {
			return hasShutdown.load();
		}

		/// <summary>
		/// the function that starts the core. has to be called via std::thread 
		/// </summary>
		/// <returns></returns>
		bool StartCore() noexcept;


		vCoreMode getMode()const noexcept;
		[[nodiscard]] uint64_t GetRegisterValue(INSTRUCTIONS::TargetRegister reg) const;
		[[nodiscard]] uint64_t GetRegisterValue(RegisterID reg) const noexcept;
		void SetRegisterValue(INSTRUCTIONS::TargetRegister reg, uint64_t value);
		void SetRegisterValue(RegisterID reg, uint64_t value) noexcept;
		[[nodiscard]] std::shared_ptr<MEMORY::MemoryBus> GetMemoryBus()const noexcept {
			return memoryBus;
		}
		[[nodiscard]] static std::string getSubregisterFromSize(SYSTEM::CPU::INSTRUCTIONS::TargetRegister reg, uint8_t bits);

		static void PrintInstruction(const INSTRUCTIONS::Instruction& instruction)noexcept;
	};

}//  namespace X86_64_EMU_SOFT::SYSTEM::CPU