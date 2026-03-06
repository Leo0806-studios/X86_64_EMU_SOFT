#include "SYSTEM/IO_DEVICES/MAIN_MEMORY_DEVICE.h"
namespace X86_64_EMU_SOFT::SYSTEM::IO_DEVICEs {

uint8_t MainMemoryDevice::Read8(uint64_t offset) const noexcept
{
	ValidateDeviceAccess(offset, 1, memory.size());
	return memory[offset];
}

uint16_t MainMemoryDevice::Read16(uint64_t offset) const noexcept
{
	return 0;
}

uint32_t MainMemoryDevice::Read32(uint64_t offset) const noexcept
{
	return 0;
}

uint64_t MainMemoryDevice::Read64(uint64_t offset) const noexcept
{
	return 0;
}

void MainMemoryDevice::Write8(uint64_t offset, uint8_t value) noexcept
{
}

void MainMemoryDevice::Write16(uint64_t offset, uint16_t value) noexcept
{
}

void MainMemoryDevice::Write32(uint64_t offset, uint32_t value) noexcept
{
}

void MainMemoryDevice::Write64(uint64_t offset, uint64_t value) noexcept
{
}

}