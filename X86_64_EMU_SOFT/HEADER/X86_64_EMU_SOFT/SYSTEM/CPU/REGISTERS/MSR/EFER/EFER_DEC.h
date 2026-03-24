#pragma once
#include <cstdint>
#include "HELPERS/GET_BIT.h"
#include "HELPERS/SET_BIT.h"
#include <SYSTEM/CPU/REGISTERS/REGISTER_BASE.h>
namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS
{
	class EFER:public Register
	{
		uint64_t storage=0;
	public:
		 void SetValue(uint64_t value) noexcept {
			storage = value;
		}
		[[nodiscard]] uint64_t GetValue() const noexcept {
			return storage;
		}
		void SetNXE(bool enabled) noexcept;
		[[nodiscard]] bool GetNXE() const noexcept;
		void SetLMA(bool enabled) noexcept;
		[[nodiscard]] bool GetLMA() const noexcept;
		void SetLME(bool enabled) noexcept;
		[[nodiscard]] bool GetLME() const noexcept;
		void SetSCE(bool enabled) noexcept;
		[[nodiscard]] bool GetSCE() const noexcept;
	};
}// namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS