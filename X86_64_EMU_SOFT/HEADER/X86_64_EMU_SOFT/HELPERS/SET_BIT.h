#pragma once
#include <cstdint>
#include <limits>
namespace X86_64_EMU_SOFT
{
	namespace HELPERS
	{

		[[nodiscard]]inline uint64_t  SetBit(uint64_t value, uint8_t bit, bool set) noexcept {
			return (value & ~(1ULL << bit)) | (uint64_t((set ? 1ULL : 0ULL)) << bit);
		}
		[[nodiscard]] inline uint32_t SetBit(uint32_t value, uint8_t bit, bool set) noexcept {
			__assume(bit < 32);
			return (value & ~(1ULL << bit)) | (uint32_t((set ? 1ULL : 0ULL)) << bit);
		}
		[[nodiscard]] inline uint16_t SetBit(uint16_t value, uint8_t bit, bool set) noexcept {
			return static_cast< uint16_t>( (value & ~(uint16_t(1) << bit)) | (uint16_t((set ? 1ULL : 0ULL)) << bit));
		}
		[[nodiscard]] inline uint8_t SetBit(uint8_t value, uint8_t bit, bool set) noexcept {
			return static_cast<uint8_t>((value & ~(1ull << bit)) | (uint8_t((set ? 1ULL : 0ULL)) << bit));
		}
	}
}