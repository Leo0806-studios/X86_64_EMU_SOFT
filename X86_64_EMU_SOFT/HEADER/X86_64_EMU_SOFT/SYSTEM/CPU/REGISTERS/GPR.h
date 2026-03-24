#pragma once
#include <cstdint>
#include <SYSTEM/CPU/REGISTERS/REGISTER_BASE.h>
namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS {
	class GPR :public Register{
		uint64_t storage=0;
	public:
		 void SetValue(uint64_t value) noexcept {
			storage = value;

		}

		[[nodiscard]]  uint64_t GetValue() const noexcept {
			return storage;
		}

	};
}// namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS

