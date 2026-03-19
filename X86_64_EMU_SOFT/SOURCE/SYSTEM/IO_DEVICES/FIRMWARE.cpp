#include <cstdint>
#include <tuple>
#include <utility>
#include <vector>
#include "SYSTEM/IO_DEVICES/DEVICE_BASE.h"
#include "SYSTEM/IO_DEVICES/FIRMWARE.h"

X86_64_EMU_SOFT::SYSTEM::IO_DEVICES::FirmwareRomDevice::FirmwareRomDevice(std::vector<uint8_t> data) noexcept :memory(std::move(data))
{
}

uint8_t X86_64_EMU_SOFT::SYSTEM::IO_DEVICES::FirmwareRomDevice::Read8(uint64_t offset) const noexcept//NOLINT(bugprone-exception-escape)
{
   // ValidateDeviceAccess(offset, 1, memory.size())
    return memory[offset];
}

uint16_t X86_64_EMU_SOFT::SYSTEM::IO_DEVICES::FirmwareRomDevice::Read16(uint64_t offset) const noexcept//NOLINT(bugprone-exception-escape)
{
    ValidateDeviceAccess(offset, 2, memory.size())
    uint16_t value = 0;
    value |= memory[offset];
	value |= static_cast<uint16_t>(memory[offset + 1]) << 8U;//NOLINT(hicpp-signed-bitwise)
    return value;
}

uint32_t X86_64_EMU_SOFT::SYSTEM::IO_DEVICES::FirmwareRomDevice::Read32(uint64_t offset) const noexcept//NOLINT(bugprone-exception-escape)
{
    ValidateDeviceAccess(offset, 4, memory.size())
    uint32_t value = 0;
    value |= memory[offset];
    value |= static_cast<uint32_t>(memory[offset + 1]) << 8U;
    value |= static_cast<uint32_t>(memory[offset + 2]) << 16U;
    value |= static_cast<uint32_t>(memory[offset + 3]) << 24U;
    return value;
}

uint64_t X86_64_EMU_SOFT::SYSTEM::IO_DEVICES::FirmwareRomDevice::Read64(uint64_t offset) const noexcept//NOLINT(bugprone-exception-escape)
{
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

void X86_64_EMU_SOFT::SYSTEM::IO_DEVICES::FirmwareRomDevice::Write8(uint64_t offset, uint8_t value) noexcept 
{
    std::ignore = offset;
    std::ignore = value; 
}

void X86_64_EMU_SOFT::SYSTEM::IO_DEVICES::FirmwareRomDevice::Write16(uint64_t offset, uint16_t value) noexcept
{
    std::ignore = offset;
    std::ignore = value;
}

void X86_64_EMU_SOFT::SYSTEM::IO_DEVICES::FirmwareRomDevice::Write32(uint64_t offset, uint32_t value) noexcept
{
    std::ignore = offset;
    std::ignore = value;
}

void X86_64_EMU_SOFT::SYSTEM::IO_DEVICES::FirmwareRomDevice::Write64(uint64_t offset, uint64_t value) noexcept
{
    std::ignore = offset;
    std::ignore = value;
}
