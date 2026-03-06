#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include "SYSTEM/IO_DEVICES/DEVICE_BASE.h"
namespace X86_64_EMU_SOFT::SYSTEM::MEMORY
{
	class MemoryBus {
	private:
		struct PageEntry
		{
			IO_DEVICEs::DeviceBase* device;
			uint64_t pageOffset;//offset inside the current device (this allows for devices with multiple regions to be mapped to the memory bus)

		};
		std::vector<PageEntry> MemoryPages;
		PageEntry* FindAccesedDevice(uint64_t address) const noexcept;
		std::vector<std::shared_ptr<IO_DEVICEs::DeviceBase>> RegisteredDevices;

	public:
		/// <summary>
		/// registers a io device to the Memory Bus. The device will be mapped to the memory bus starting at preferedBaseAdress and will occupy pageCount pages. 
		/// If preferedBaseAdress is already occupied by another device, the function will try to find the next available address. 
		/// If no available address is found, the function will return false. The function returns true if the device was successfully registered, false otherwise.
		/// if not enough consecutive pages are availible it will spill the devices region. the device will get readWrites as if it were a single region
		/// </summary>
		/// <param name="device"></param>
		/// <param name="pageCount"></param>
		/// <param name="preferedBaseAdress">has to be page aligned </param>
		/// <returns></returns>
		[[nodiscard]] bool RegisterIODevice(std::shared_ptr<IO_DEVICEs::DeviceBase> device, uint64_t pageCount,uint64_t preferedBaseAdress);
		[[nodsicard]] bool UnregisterIODevice(std::shared_ptr<IO_DEVICEs::DeviceBase> device) noexcept;

		uint8_t Read8(uint64_t address) const noexcept;
		uint16_t Read16(uint64_t address) const noexcept;
		uint32_t Read32(uint64_t address) const noexcept;
		uint64_t Read64(uint64_t address) const noexcept;
		void Write8(uint64_t address, uint8_t value) noexcept;
		void Write16(uint64_t address, uint16_t value) noexcept;
		void Write32(uint64_t address, uint32_t value) noexcept;
		void Write64(uint64_t address, uint64_t value) noexcept;
	};
}