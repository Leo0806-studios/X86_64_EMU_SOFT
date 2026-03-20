#pragma once
#include <vector>
#include <cstdint>
#include "DEVICE_BASE.h"
namespace X86_64_EMU_SOFT::SYSTEM::IO_DEVICES {
	class FirmwareRomDevice final :public DeviceBase {
		std::vector<uint8_t> memory;
	public:
		[[nodiscard]] explicit FirmwareRomDevice(std::vector<uint8_t> data) noexcept;

		// Inherited via DeviceBase
		[[nodiscard]]uint8_t Read8(uint64_t offset) const noexcept override;

		[[nodiscard]]uint16_t Read16(uint64_t offset) const noexcept override;

		[[nodiscard]]uint32_t Read32(uint64_t offset) const noexcept override;

		[[nodiscard]]uint64_t Read64(uint64_t offset) const noexcept override;

		void Write8(uint64_t offset, uint8_t value) noexcept override;

		void Write16(uint64_t offset, uint16_t value) noexcept override;

		void Write32(uint64_t offset, uint32_t value) noexcept override;

		void Write64(uint64_t offset, uint64_t value) noexcept override;
	};
}// namespace X86_64_EMU_SOFT::SYSTEM::IO_DEVICES