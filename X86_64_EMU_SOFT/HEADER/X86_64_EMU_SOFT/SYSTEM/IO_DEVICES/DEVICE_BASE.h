#pragma once
#include <cstdint>
#include <print>
#include <intrin.h>
namespace X86_64_EMU_SOFT::SYSTEM::IO_DEVICES
{
	struct MMOIInfo{
		uint64_t PreferedBaseAddress;
		uint64_t PageCount;
	};
	class DeviceBase//NOLINT(cppcoreguidelines-special-member-functions,hicpp-special-member-functions)
	{
	public:
		virtual ~DeviceBase() = default;
		[[nodiscard]]virtual uint8_t Read8(uint64_t offset) const noexcept = 0;
		[[nodiscard]]virtual uint16_t Read16(uint64_t offset) const noexcept = 0;
		[[nodiscard]]virtual uint32_t Read32(uint64_t offset) const noexcept = 0;
		[[nodiscard]]virtual uint64_t Read64(uint64_t offset) const noexcept = 0;
		virtual void Write8(uint64_t offset, uint8_t value) noexcept = 0;
		virtual void Write16(uint64_t offset, uint16_t value) noexcept = 0;
		virtual void Write32(uint64_t offset, uint32_t value) noexcept = 0;
		virtual void Write64(uint64_t offset, uint64_t value) noexcept = 0;

	};
	
#ifdef _DEBUG
	//all parameters must be unsigned
#define ValidateDeviceAccess(offset,size, maxSize) \
	if ((offset) + (size) > (maxSize)) { \
		std::print("device acces out of range\n"); \
		__debugbreak();\
		while(true){}\
	}

#else
#define ValidateDeviceAccess(offset,size, maxSize) 

#endif // DEBUG

}// namespace X86_64_EMU_SOFT::SYSTEM::IO_DEVICES