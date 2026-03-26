#pragma once
#include <cstdint>
#include <type_traits>
#include <SYSTEM/CPU/REGISTERS/REGISTER_BASE.h>
namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS {
	class [[nodiscard("discarding a register is at best a performance issue and at worst a logic bug")]] GPR :public Register {
		uint64_t storage=0;

		
	public:
		 void SetValue(uint64_t value) noexcept {
			storage = value;
			
		}

		[[nodiscard]]  uint64_t GetValue() const noexcept {
			return storage;
		}
		void SetLow8Bits(uint8_t value) noexcept {
			storage = (storage & 0xFFFFFFFFFFFFFF00ULL) | value;
		}
		[[nodiscard]] uint8_t GetLow8Bits() const noexcept {
			return static_cast<uint8_t>(storage & 0xFFU);
		}
		void SetLow16Bits(uint16_t value) noexcept {
			storage = (storage & 0xFFFFFFFFFFFF0000ULL) | value;
		}
		[[nodiscard]] uint16_t GetLow16Bits() const noexcept {
			return static_cast<uint16_t>(storage & 0xFFFFU);
		}
		void SetLow32Bits(uint32_t value) noexcept {
			storage = value;
		}
		[[nodiscard]] uint32_t GetLow32Bits() const noexcept {
			return static_cast<uint32_t>(storage & 0xFFFFFFFF);
		}
		void SetHigh8Bits(uint8_t value) noexcept {
			storage = (storage & 0xFFFFFFFFFFFF00FFULL) | (static_cast<uint64_t>(value) << 8);
		}
		[[nodiscard]] uint8_t GetHigh8Bits() const noexcept {
			return static_cast<uint8_t>((storage >> 8U) & 0xFFU);
		}
		// overloaded operators for 64 bit operations
		void operator +=(uint64_t value) noexcept {
			storage += value;
		}
		void operator -=(uint64_t value) noexcept {
			storage -= value;
		}
		void operator &=(uint64_t value) noexcept {
			storage &= value;
		}
		void operator |=(uint64_t value) noexcept {
			storage |= value;
		}
		void operator ^=(uint64_t value) noexcept {
			storage ^= value;
		}
		void operator ~(){
			storage = ~storage;
		}
		[[nodiscard]] explicit operator uint64_t() const noexcept {
			return storage;
		}


	};
}// namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS

