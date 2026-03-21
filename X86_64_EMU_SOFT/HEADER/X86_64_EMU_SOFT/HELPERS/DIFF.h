#pragma once
#include <bitset>
#include  <cstdint>
namespace X86_64_EMU_SOFT::HELPERS
{
	[[nodiscard]] constexpr uint64_t Diff(uint64_t a, uint64_t b) noexcept {
		return a > b ? a - b : b - a;
	}
	[[nodiscard]] constexpr uint32_t Diff(uint32_t a, uint32_t b) noexcept {
		return a > b ? a - b : b - a;
	}
	[[nodiscard]] constexpr uint16_t Diff(uint16_t a, uint16_t b) noexcept {
		return static_cast<uint16_t>((a > b) ? (a - b) : (b - a));
	}
	[[nodiscard]] constexpr uint8_t Diff(uint8_t a, uint8_t b) noexcept {
		return static_cast<uint8_t>((a > b) ? (a - b) : (b - a));
	}



}// namespace X86_64_EMU_SOFT::HELPERS