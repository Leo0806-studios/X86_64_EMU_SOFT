#pragma once
#include <cstdint>
namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS
{
	class RIP
	{
		uint64_t storage;
	public:
		void SetValue(uint64_t value) noexcept {
			storage = value;
		}
		[[nodiscard]] uint64_t GetValue() const noexcept {
			return storage;
		}

		void Increment(uint8_t len)noexcept {
			storage += len;
		}
		
	};
}// namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS