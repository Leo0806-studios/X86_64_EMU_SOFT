#pragma once
import <cstdint>;
import <bitset>;

namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS {
	class GPR {
		uint64_t storage;
	public:
		inline void SetValue(uint64_t value) noexcept {
			storage = value;

		}
		inline void SetValue(uint64_t value) noexcept {
			storage = value;
		}
		[[nodiscard]] inline uint64_t GetValue() const noexcept {
			return storage;
		}

	};
}

