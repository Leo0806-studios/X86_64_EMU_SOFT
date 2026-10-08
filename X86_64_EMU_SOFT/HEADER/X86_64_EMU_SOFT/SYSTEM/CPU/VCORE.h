#pragma once
#include <atomic>
#include <cstdint>
#include <HELPERS/MACROS.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <SYSTEM/CPU/REGISTERS/REGISTER_BASE.h>
#include <tracy/Tracy.hpp>
#include <utility>
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/REGISTERS/CONTROLL_REGISTERS/CR0.h"
#include "SYSTEM/CPU/REGISTERS/GPR.h"
#include "SYSTEM/CPU/REGISTERS/MSR/EFER.h"
#include "SYSTEM/CPU/REGISTERS/RIP.h"
#include "SYSTEM/MEMORY/MEMORY.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU
{

	enum class vCoreMode :uint8_t {
		realMode,
		protectedMode,
		longMode,
	};
	inline vCoreMode IntToMode(int modeBits)noexcept {
		switch (modeBits) {
			case 16: return vCoreMode::realMode;
			case 32: return vCoreMode::protectedMode;
			case 64: return vCoreMode::longMode;
			default:__assume(false);
		}

	}

	class VirtualCore//NOSONAR
	{
		class Cache {
		public:
			enum class CacheLineFlags :uint8_t {
				Valid = 0b00000001,
				Dirty = 0b00000010,
				Stale = 0b00000100,
				Reserved2= 0b00001000,
				Reserved3 = 0b00010000,
				Reserved4 = 0b00100000
			};
		private:
			uint64_t cacheClock = 0;//independend of the core clock. used to implement LRU cache replacement policy
			MEMORY::MemoryBus* memoryBus;
			struct CacheLine {
				uint64_t baseAddress=0;
				std::array<uint8_t, 64> data={{0}};
				uint8_t flags = 0u | std::to_underlying(CacheLineFlags::Stale);
				uint64_t lastAccessed = 0;
			};
			std::array< CacheLine, 64> cacheLines;
			int32_t getCacheLineIndex(uint64_t allignedAddress) noexcept;
			uint16_t nextRefresh = 0;//the cache line that gets replaced the next time a adress from a not in cach line gets loaded. round robin replacement policy for now
			
			/// <summary>
			/// selects the line to be used for further cache operations. returns the cache line index that was selected. if no cache line was selected returns -1
			/// it transparently fetches bytes from the memory bus if the line is not in cache or stale. if the cache is full it evicts a line according to the replacement policy
			/// </summary>
			/// <returns></returns>
			int64_t selectCacheLine(uint64_t allignedAddress) noexcept;
		public:
			explicit Cache(MEMORY::MemoryBus* memoryBus)noexcept : memoryBus(memoryBus), cacheLines() {//TODO : make cache size configurable
				
				for (auto& line : cacheLines) {
					line.baseAddress = 0;
					line.flags = 0;
					line.lastAccessed = 0;
				}

			}
			[[nodiscard]] uint8_t Read8(uint64_t address) noexcept;
			[[nodiscard]] uint16_t Read16(uint64_t address) noexcept;
			[[nodiscard]] uint32_t Read32(uint64_t address) noexcept;
			[[nodiscard]] uint64_t Read64(uint64_t address) noexcept;
			void Write8(uint64_t address, uint8_t value)noexcept;
			void Write16(uint64_t address, uint16_t value)noexcept;
			void Write32(uint64_t address, uint32_t value)noexcept;
			void Write64(uint64_t address, uint64_t value)noexcept;
			//no write  cache yet


			std::string ToString()const;
		};

		friend class DecodingEngine;
		friend class ExecutionEngine;
		mutable Cache cache;
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
		void decodeInstruction(INSTRUCTIONS::Instruction& instruction);
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
		[[nodiscard]] REGISTERS::Register& GetRegister(INSTRUCTIONS::TargetRegister reg)noexcept;

		vCoreMode getMode()const noexcept
		{
			DeepZoneScoped;//NOLINT
			vCoreMode ret = vCoreMode::realMode;
			if (EFER.GetLMA() && !CR0.GetPE()) {
				ret = vCoreMode::longMode;
			}
			else if (!EFER.GetLMA() && CR0.GetPE()) {
				ret = vCoreMode::protectedMode;
			}
			return ret;

		}
		[[nodiscard]] uint8_t GetDefaultOperandSize() const noexcept
		{
			DeepZoneScoped;//NOLINT
			//TODO : implement real Segmentation checks
			switch (getMode()) {
				case vCoreMode::realMode:return 16;
				case vCoreMode::protectedMode:
				case vCoreMode::longMode:return 32;
				default:__assume(false);
			}
		}

		/// <summary>
		/// sets the entire FLAGS register(EEFLAGS,RFLAGS) to the provided value.
		/// </summary>
		/// <param name="flags"></param>
		void SetFlags(uint64_t flags)noexcept;

		/// <summary>
		/// sets FLAGs(EEFLAGS,RFLAGS)according to Value. bits set to one in the KeepMask will be carried over unchagend from the original Flags value. bits set to zero in the KeepMask will be overwritten by the corresponding bit in the value parameter
		/// </summary>
		/// <param name="keepMask"></param>
		/// <param name="value"></param>
		void Setflags(uint64_t keepMask, uint64_t value)noexcept;

		/// <summary>
		/// Gets the value of the entire FLAGS register (EFLAGS,RFLAGS)
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] uint64_t GetFlags() const noexcept;

		/// <summary>
		/// Sets a singular Flag bit in the FLAGS register (EFLAGS,RFLAGS) according to "enabled".
		/// </summary>
		/// <param name="flagBit"></param>
		/// <param name="enabled"></param>
		void SetFlag(uint8_t flagBit, bool enabled) noexcept;

		/// <summary>
		/// gets a singualr Flag bit from the FLAGS register (EFLAGS,RFLAGS)
		/// </summary>
		/// <param name="flagBit"></param>
		/// <returns></returns>
		[[nodiscard]] bool GetFlag(uint8_t flagBit) const noexcept;


		[[nodiscard]] std::string getSubregisterFromSize(const REGISTERS::Register* registerPtr, uint8_t bits, bool high);
		/// <summary>
		/// writes "sizeBytes" amount of bytes from "value" to the memory bus while performing all necesary validation of the target address
		/// sizebytes must one of 1,2,4,8
		/// </summary>
		/// <param name="address"></param>
		/// <param name="value"></param>
		/// <param name="sizeBytes"></param>
		void WriteBytes(uint64_t address, const uint64_t value, uint8_t sizeBytes)noexcept;

		/// <summary>
		/// fetches "sizeBtes" amount of bytes.
		/// sizeBytes must be one of 1,2,4,8
		/// </summary>
		/// <param name="address"></param>
		/// <param name="sizeBytes"></param>
		/// <returns></returns>
		[[nodiscard]] uint64_t FetchBytes(uint64_t address, uint8_t sizeBytes) const noexcept;
		void PrintInstruction(const INSTRUCTIONS::Instruction& instruction) const;

	};

}//  namespace X86_64_EMU_SOFT::SYSTEM::CPU
