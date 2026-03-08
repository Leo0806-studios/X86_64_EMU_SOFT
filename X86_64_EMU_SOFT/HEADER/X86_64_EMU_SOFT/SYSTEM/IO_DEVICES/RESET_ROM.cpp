#include "RESET_ROM.h"

X86_64_EMU_SOFT::SYSTEM::IO_DEVICEs::ResetROMDevice::ResetROMDevice(std::vector<uint8_t> data) noexcept :memory(std::move(data))
{
}

uint8_t X86_64_EMU_SOFT::SYSTEM::IO_DEVICEs::ResetROMDevice::Read8(uint64_t offset) const noexcept
{
	ValidateDeviceAccess(offset, 1, memory.size());
	return memory[offset];
}

uint16_t X86_64_EMU_SOFT::SYSTEM::IO_DEVICEs::ResetROMDevice::Read16(uint64_t offset) const noexcept
{
	ValidateDeviceAccess(offset, 2, memory.size());
	uint16_t value = 0;
	value |= memory[offset];
	value |= static_cast<uint16_t>(memory[offset + 1]) << 8;
	return value;
}

uint32_t X86_64_EMU_SOFT::SYSTEM::IO_DEVICEs::ResetROMDevice::Read32(uint64_t offset) const noexcept
{
	ValidateDeviceAccess(offset, 4, memory.size());
	uint32_t value = 0;
	value |= memory[offset];
	value |= static_cast<uint32_t>(memory[offset + 1]) << 8;
	value |= static_cast<uint32_t>(memory[offset + 2]) << 16;
	value |= static_cast<uint32_t>(memory[offset + 3]) << 24;
	return value;
}

uint64_t X86_64_EMU_SOFT::SYSTEM::IO_DEVICEs::ResetROMDevice::Read64(uint64_t offset) const noexcept
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

void X86_64_EMU_SOFT::SYSTEM::IO_DEVICEs::ResetROMDevice::Write8(uint64_t offset, uint8_t value) noexcept
{
	std::ignore = offset;
	std::ignore = value;
}

void X86_64_EMU_SOFT::SYSTEM::IO_DEVICEs::ResetROMDevice::Write16(uint64_t offset, uint16_t value) noexcept
{
	std::ignore = offset;
	std::ignore = value;
}

void X86_64_EMU_SOFT::SYSTEM::IO_DEVICEs::ResetROMDevice::Write32(uint64_t offset, uint32_t value) noexcept
{
	std::ignore = offset;
	std::ignore = value;
}


void X86_64_EMU_SOFT::SYSTEM::IO_DEVICEs::ResetROMDevice::Write64(uint64_t offset, uint64_t value) noexcept
{
	std::ignore = offset;
	std::ignore = value;
}

