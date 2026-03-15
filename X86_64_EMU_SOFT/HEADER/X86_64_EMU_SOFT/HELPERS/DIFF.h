#pragma once
import <cstdint>;
import <bitset>;
namespace X86_64_EMU_SOFT
{
	namespace HELPERS {
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

		[[nodiscard]] std::bitset<64> Diff(const std::bitset<64>& a, const std::bitset<64>& b) noexcept {
			std::bitset<64> result;
			for (size_t i = 0; i < 64; i++) {
				result[i] = a[i] ^ b[i];
			}
			return result;
		}
		[[nodiscard]] std::bitset <32> Diff(const std::bitset<32>& a, const std::bitset<32>& b) noexcept {
			std::bitset<32> result;
			for (size_t i = 0; i < 32; i++) {
				result[i] = a[i] ^ b[i];
			}
			return result;
		}
		[[nodiscard]] std::bitset<16> Diff(const std::bitset<16>& a, const std::bitset<16>& b) noexcept {
			std::bitset<16> result;
			for (size_t i = 0; i < 16; i++) {
				result[i] = a[i] ^ b[i];
			}
			return result;
		}
		[[nodiscard]] std::bitset<8> Diff(const std::bitset<8>& a, const std::bitset<8>& b) noexcept {
			std::bitset<8> result;
			for (size_t i = 0; i < 8; i++) {
				result[i] = a[i] ^ b[i];
			}
			return result;
		}
		
	}
}