#pragma once
#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>
#include "SYSTEM/IO_DEVICES/DEVICE_BASE.h"
namespace X86_64_EMU_SOFT::SYSTEM::MEMORY
{
	class MemoryBus {
	private:


		struct PageEntry
		{
			struct PageSection {
				IO_DEVICES::DeviceBase* device = nullptr;
				uint64_t DeviceOffset = 0;
				uint32_t pageOffset = 0;
				uint32_t size = 4096;
			};
			std::vector<PageSection> Sections = std::vector<PageSection>(1);
			bool IsPageContiguous()const noexcept;
			uint64_t GetFrreBytesInPage()const noexcept;
			std::pair<uint64_t/*in page offset*/, uint64_t/*size Bytes*/> GetNextFreeSection()const noexcept;
			void SortSections() noexcept;

		};



		std::vector<PageEntry> MemoryPages;

		struct DeviceInfos
		{
			std::shared_ptr<IO_DEVICES::DeviceBase> device;
			uint64_t sizeBytes = 0;
			uint64_t baseAdress = 0;//holds prefered base adress untill BuildPageTable is called. after that it holds the actual base adress of the device
		};
		std::vector<DeviceInfos> RegisteredDevices;
	public:
		[[nodiscard]] bool RegisterIODevice(std::shared_ptr<IO_DEVICES::DeviceBase> device, uint64_t sizeBytes, uint64_t preferedBaseAdress);

		[[nodiscard]] bool BuildPageTable() noexcept;
		[[nodiscard]] bool UnregisterIODevice(std::shared_ptr<IO_DEVICES::DeviceBase> device) noexcept;

		[[nodiscard]] uint8_t Read8(uint64_t address) const noexcept;
		[[nodiscard]] uint16_t Read16(uint64_t address) const noexcept;
		[[nodiscard]] uint32_t Read32(uint64_t address) const noexcept;
		[[nodiscard]] uint64_t Read64(uint64_t address) const noexcept;
		void Write8(uint64_t address, uint8_t value) noexcept;
		void Write16(uint64_t address, uint16_t value) noexcept;
		void Write32(uint64_t address, uint32_t value) noexcept;
		void Write64(uint64_t address, uint64_t value) noexcept;


		void PrintMemoryMap() const noexcept;
	};
}