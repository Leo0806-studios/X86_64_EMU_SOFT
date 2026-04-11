#pragma once
#include <cstdint>

namespace X86_64_EMU_SOFT::HELPERS
{

	[[nodiscard]] constexpr bool GetBit(uint64_t value, uint8_t bit) noexcept {
		return (value >> bit) & 1U;
	}
	[[nodiscard]] constexpr bool GetBit(uint32_t value, uint8_t bit) noexcept {
		return (value >> bit) & 1U;
	}
	[[nodiscard]] constexpr bool GetBit(uint16_t value, uint8_t bit) noexcept {
		return (value >> bit) & 1U;//NOLINT(hicpp-signed-bitwise)
	}
	[[nodiscard]] constexpr bool GetBit(uint8_t value, uint8_t bit) noexcept {
		return (value >> bit) & 1U;//NOLINT(hicpp-signed-bitwise) 
		
	}
}// namespace X86_64_EMU_SOFT::HELPERS