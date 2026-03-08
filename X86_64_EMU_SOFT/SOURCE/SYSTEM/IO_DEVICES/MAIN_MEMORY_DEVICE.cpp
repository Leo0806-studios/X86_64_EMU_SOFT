#include "SYSTEM/IO_DEVICES/MAIN_MEMORY_DEVICE.h"
namespace X86_64_EMU_SOFT::SYSTEM::IO_DEVICEs {
	MainMemoryDevice::MainMemoryDevice(size_t sizeKB) noexcept
	{
		memory.resize(sizeKB * 1024, 0);
	}
	uint8_t MainMemoryDevice::Read8(uint64_t offset) const noexcept
{
	ValidateDeviceAccess(offset, 1, memory.size());
	return memory[offset];
}

uint16_t MainMemoryDevice::Read16(uint64_t offset) const noexcept
{
	ValidateDeviceAccess(offset, 2, memory.size());
	uint16_t value = 0;
	value |= memory[offset];
	value |= static_cast<uint16_t>(memory[offset + 1]) << 8;
	return value;
}

uint32_t MainMemoryDevice::Read32(uint64_t offset) const noexcept
{
	ValidateDeviceAccess(offset, 4, memory.size());
	uint32_t value = 0;
	value |= memory[offset];
	value |= static_cast<uint32_t>(memory[offset + 1]) << 8;
	value |= static_cast<uint32_t>(memory[offset + 2]) << 16;
	value |= static_cast<uint32_t>(memory[offset + 3]) << 24;
	return value;
}

uint64_t MainMemoryDevice::Read64(uint64_t offset) const noexcept
{
	ValidateDeviceAccess(offset, 8, memory.size());
	uint64_t value = 0;
	value |= memory[offset];
	value |= static_cast<uint64_t>(memory[offset + 1]) << 8;
	value |= static_cast<uint64_t>(memory[offset + 2]) << 16;
	value |= static_cast<uint64_t>(memory[offset + 3]) << 24;
	value |= static_cast<uint64_t>(memory[offset + 4]) << 32;
	value |= static_cast<uint64_t>(memory[offset + 5]) << 40;
	value |= static_cast<uint64_t>(memory[offset + 6]) << 48;
	value |= static_cast<uint64_t>(memory[offset + 7]) << 56;
	return value;
}

void MainMemoryDevice::Write8(uint64_t offset, uint8_t value) noexcept
{
	ValidateDeviceAccess(offset, 1, memory.size());
	memory[offset] = value;
}

void MainMemoryDevice::Write16(uint64_t offset, uint16_t value) noexcept
{
	ValidateDeviceAccess(offset, 2, memory.size());
	memory[offset] = value & 0xFFULL;
	memory[offset + 1] = (value >> 8) & 0xFFULL;
}

void MainMemoryDevice::Write32(uint64_t offset, uint32_t value) noexcept
{
	ValidateDeviceAccess(offset, 4, memory.size());
	memory[offset] = value & 0xFF;
	memory[offset + 1] = (value >> 8) & 0xFF;
	memory[offset + 2] = (value >> 16) & 0xFF;
	memory[offset + 3] = (value >> 24) & 0xFF;
}

void MainMemoryDevice::Write64(uint64_t offset, uint64_t value) noexcept
{
	ValidateDeviceAccess(offset, 8, memory.size());
	memory[offset] = value & 0xFF;
	memory[offset + 1] = (value >> 8) & 0xFF;
	memory[offset + 2] = (value >> 16) & 0xFF;
	memory[offset + 3] = (value >> 24) & 0xFF;
	memory[offset + 4] = (value >> 32) & 0xFF;
	memory[offset + 5] = (value >> 40) & 0xFF;
	memory[offset + 6] = (value >> 48) & 0xFF;
	memory[offset + 7] = (value >> 56) & 0xFF;
}

}