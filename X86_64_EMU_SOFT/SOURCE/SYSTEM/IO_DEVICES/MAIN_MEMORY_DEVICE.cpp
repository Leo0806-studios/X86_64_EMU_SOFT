#include <cstdint>
#include <tracy/Tracy.hpp>
#include "SYSTEM/IO_DEVICES/MAIN_MEMORY_DEVICE.h"
#include "SYSTEM/IO_DEVICES/DEVICE_BASE.h"
#include <HELPERS/MACROS.h>
#include <HELPERS/REDEFINE_MACROS.h>
namespace X86_64_EMU_SOFT::SYSTEM::IO_DEVICES {
	MainMemoryDevice::MainMemoryDevice(size_t sizeKB) noexcept//NOLINT(bugprone-exception-escape)
	{
		memory.resize(sizeKB * 1024, 0);
	}
	uint8_t MainMemoryDevice::Read8(const uint64_t offset) const noexcept//NOLINT(bugprone-exception-escape)
	{
		DeepZoneScoped;
		ValidateDeviceAccess(offset, 1, memory.size())
			return memory[offset];
	}

	uint16_t MainMemoryDevice::Read16(uint64_t offset) const noexcept//NOLINT(bugprone-exception-escape)
	{
		DeepZoneScoped;
		ValidateDeviceAccess(offset, 2, memory.size())
			uint16_t value = 0;
		value |= memory[offset];
		value |= static_cast<uint16_t>(memory[offset + 1]) << 8U;//NOLINT(hicpp-signed-bitwise)
		return value;
	}

	uint32_t MainMemoryDevice::Read32(uint64_t offset) const noexcept//NOLINT(bugprone-exception-escape)
	{
		DeepZoneScoped;
		ValidateDeviceAccess(offset, 4, memory.size())
			uint32_t value = 0;
		value |= memory[offset];
		value |= static_cast<uint32_t>(memory[offset + 1]) << 8U;
		value |= static_cast<uint32_t>(memory[offset + 2]) << 16U;
		value |= static_cast<uint32_t>(memory[offset + 3]) << 24U;
		return value;
	}

	uint64_t MainMemoryDevice::Read64(uint64_t offset) const noexcept//NOLINT(bugprone-exception-escape)
	{
		DeepZoneScoped;
		ValidateDeviceAccess(offset, 8, memory.size())
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

	void MainMemoryDevice::Write8(uint64_t offset, uint8_t value) noexcept//NOLINT(bugprone-exception-escape)
	{
		DeepZoneScoped;
		ValidateDeviceAccess(offset, 1U, memory.size())
			memory[offset] = value;
	}

	void MainMemoryDevice::Write16(uint64_t offset, uint16_t value) noexcept//NOLINT(bugprone-exception-escape)
	{
		DeepZoneScoped;
		ValidateDeviceAccess(offset, 2U, memory.size())
			memory[offset] = value & 0xFFULL;
		memory[offset + 1] = (value >> 8U) & 0xFFULL;//NOLINT(hicpp-signed-bitwise)
	}

	void MainMemoryDevice::Write32(uint64_t offset, uint32_t value) noexcept//NOLINT(bugprone-exception-escape)
	{
		DeepZoneScoped;
		ValidateDeviceAccess(offset, 4U, memory.size())
			memory[offset] = value & 0xFFU;
		memory[offset + 1] = (value >> 8U) & 0xFFU;
		memory[offset + 2] = (value >> 16U) & 0xFFU;
		memory[offset + 3] = (value >> 24U) & 0xFFU;
	}

	void MainMemoryDevice::Write64(uint64_t offset, uint64_t value) noexcept//NOLINT(bugprone-exception-escape)
	{
		DeepZoneScoped;
		ValidateDeviceAccess(offset, 8U, memory.size())
			memory[offset] = value & 0xFFU;
		memory[offset + 1] = (value >> 8U) & 0xFFU;
		memory[offset + 2] = (value >> 16U) & 0xFFU;
		memory[offset + 3] = (value >> 24U) & 0xFFU;
		memory[offset + 4] = (value >> 32U) & 0xFFU;
		memory[offset + 5] = (value >> 40U) & 0xFFU;
		memory[offset + 6] = (value >> 48U) & 0xFFU;
		memory[offset + 7] = (value >> 56U) & 0xFFU;
	}

}// namespace X86_64_EMU_SOFT::SYSTEM::IO_DEVICES