#pragma once
#include <cstdint>
#include <vector>
#include "SYSTEM/IO_DEVICES/DEVICE_BASE.h"
namespace X86_64_EMU_SOFT::SYSTEM::IO_DEVICES {
	class ResetROMDevice final : public DeviceBase
	{
		std::vector<uint8_t> memory;
	public:
		[[nodiscard]] explicit ResetROMDevice(std::vector<uint8_t> data) noexcept;

		// Inherited via DeviceBase
		[[nodiscard ]] uint8_t Read8(uint64_t offset) const noexcept override;
		[[nodiscard ]] uint16_t Read16(uint64_t offset) const noexcept override;
		[[nodiscard ]] uint32_t Read32(uint64_t offset) const noexcept override;
		[[nodiscard ]] uint64_t Read64(uint64_t offset) const noexcept override;




		// Inherited via DeviceBase
		void Write8(uint64_t offset, uint8_t value) noexcept override;

		void Write16(uint64_t offset, uint16_t value) noexcept override;

		void Write32(uint64_t offset, uint32_t value) noexcept override;

		void Write64(uint64_t offset, uint64_t value) noexcept override;
		[[nodiscard]] uint8_t* GetDataPtr() noexcept {
			return memory.data();
		}


	};
}// namespace X86_64_EMU_SOFT::SYSTEM::IO_DEVICES