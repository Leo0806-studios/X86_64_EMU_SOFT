#include <cstdint>
#include <tuple>
#include <utility>
#include <vector>
#include <tracy/Tracy.hpp>
#include "SYSTEM/IO_DEVICES/RESET_ROM.h"
#include "SYSTEM/IO_DEVICES/DEVICE_BASE.h"
#include <HELPERS/MACROS.h>
#include <HELPERS/REDEFINE_MACROS.h>
namespace X86_64_EMU_SOFT::SYSTEM::IO_DEVICES {

	ResetROMDevice::ResetROMDevice(std::vector<uint8_t> data) noexcept :memory(std::move(data))
	{}

	uint8_t ResetROMDevice::Read8(uint64_t offset) const noexcept//NOLINT(bugprone-exception-escape)
	{
		DeepZoneScoped;
		ValidateDeviceAccess(offset, 1U, memory.size())
		return memory[offset];
	}

	uint16_t ResetROMDevice::Read16(uint64_t offset) const noexcept//NOLINT(bugprone-exception-escape)
	{
		DeepZoneScoped;
		ValidateDeviceAccess(offset, 2U, memory.size())
		uint16_t value = 0;
		value |= memory[offset];
		value |= static_cast<uint16_t>(memory[offset + 1]) << 8U;//NOLINT(hicpp-signed-bitwise)
		return value;
	}

	uint32_t ResetROMDevice::Read32(uint64_t offset) const noexcept//NOLINT(bugprone-exception-escape)
	{
		DeepZoneScoped;
		ValidateDeviceAccess(offset, 4U, memory.size())
		uint32_t value = 0;
		value |= memory[offset];
		value |= static_cast<uint32_t>(memory[offset + 1]) << 8U;
		value |= static_cast<uint32_t>(memory[offset + 2]) << 16U;
		value |= static_cast<uint32_t>(memory[offset + 3]) << 24U;
		return value;
	}

	uint64_t ResetROMDevice::Read64(uint64_t offset) const noexcept//NOLINT(bugprone-exception-escape)
	{
		DeepZoneScoped;
		ValidateDeviceAccess(offset, 8U, memory.size())
		uint64_t value = 0;
		value |= memory[offset];
		value |= static_cast<uint64_t>(memory[offset + 1]) << 8U;
		value |= static_cast<uint64_t>(memory[offset + 2]) << 16U;
		value |= static_cast<uint64_t>(memory[offset + 3]) << 24U;
		value |= static_cast<uint64_t>(memory[offset + 4]) << 32U;
		value |= static_cast<uint64_t>(memory[offset + 5]) << 40U;
		value |= static_cast<uint64_t>(memory[offset + 6]) << 48U;
		value |= static_cast<uint64_t>(memory[offset + 7]) << 56U;
		return value;
	}

	void ResetROMDevice::Write8(uint64_t offset, uint8_t value) noexcept
	{
		DeepZoneScoped;
		std::ignore = offset;
		std::ignore = value;
	}

	void ResetROMDevice::Write16(uint64_t offset, uint16_t value) noexcept
	{
		DeepZoneScoped;
		std::ignore = offset;
		std::ignore = value;
	}

	void ResetROMDevice::Write32(uint64_t offset, uint32_t value) noexcept
	{
		DeepZoneScoped;
		std::ignore = offset;
		std::ignore = value;
	}


	void ResetROMDevice::Write64(uint64_t offset, uint64_t value) noexcept
	{
		DeepZoneScoped;
		std::ignore = offset;
		std::ignore = value;
	}


}// namespace X86_64_EMU_SOFT::SYSTEM::IO_DEVICES