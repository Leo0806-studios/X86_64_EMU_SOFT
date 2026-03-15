#pragma once
#include "EFER_DEC.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS {
	inline void EFER::SetNXE(bool enabled) noexcept
	{
		storage=HELPERS::SetBit(storage, 11, enabled);
	}
	inline bool EFER::GetNXE() const noexcept
	{
		return HELPERS::GetBit(storage,11);
	}
	inline void EFER::SetLMA(bool enabled) noexcept
	{
		storage = HELPERS::SetBit(storage, 10, enabled);
	}
	inline bool EFER::GetLMA() const noexcept
	{
		return HELPERS::GetBit(storage, 10);
	}
	inline void EFER::SetLME(bool enabled) noexcept {
		storage = HELPERS::SetBit(storage, 8, enabled);
	}

	[[nodiscard]]
	inline bool EFER::GetLME() const noexcept {
		return HELPERS::GetBit(storage, 8);
	}

	inline void EFER::SetSCE(bool enabled) noexcept
	{
		storage = HELPERS::SetBit(storage, 0, enabled);
	}

	inline bool EFER::GetSCE() const noexcept
	{
		return HELPERS::GetBit(storage, 0);
	}

}