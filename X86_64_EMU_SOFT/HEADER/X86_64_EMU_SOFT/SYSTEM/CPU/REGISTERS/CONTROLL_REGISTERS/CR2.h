#pragma once
#include <cstdint>
namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS{ 
			class CR2
			{
				uint64_t storage;
			public:
				inline void SetValue(uint64_t value) noexcept {
					storage = value;
				}
				[[nodiscard]] inline uint64_t GetValue() const noexcept {
					return storage;
				}
			};
}