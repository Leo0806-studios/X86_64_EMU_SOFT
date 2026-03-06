#pragma once
#include <cstdint>
#include "HELPERS/GET_BIT.h"
#include "HELPERS/SET_BIT.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS
{
			class EFER
			{
				uint64_t storage;
			public:
				inline void SetValue(uint64_t value) noexcept {
					storage = value;
				}
				inline uint64_t GetValue() const noexcept {
					return storage;
				}
				void SetLME(bool enabled) noexcept {
					storage = HELPERS::SetBit(storage, 8, enabled);
				}
				[[nodiscard]] bool GetLME() const noexcept {
					return HELPERS::GetBit(storage, 8);
				}
			};
}