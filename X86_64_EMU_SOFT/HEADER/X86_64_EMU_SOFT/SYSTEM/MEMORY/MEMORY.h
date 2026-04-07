#pragma once
#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>
#include <utility>
#include "SYSTEM/IO_DEVICES/DEVICE_BASE.h"
namespace X86_64_EMU_SOFT::SYSTEM::MEMORY
{
	class MemoryBus {
	private:


		struct PageEntry
		{
			struct PageSection {
				enum class DeviceTag :std::uint8_t {
					MainMemory,
					ResetRom,
					FirmwareRom,
					OtherDevice
				};
				enum class Flags : uint8_t {
					SideEffectRead = 1U << 0U,
					SideEffectWrite = 1U << 1U,
					DirectAccess = 1U << 2U,
				};

					IO_DEVICES::DeviceBase* device = nullptr;
					uint8_t* dataPtr = nullptr;
				uint8_t Flags = 0;
				uint64_t DeviceOffset = 0;
				uint32_t pageOffset = 0;
				uint32_t size = 4096;

			};
			std::vector<PageSection> Sections = std::vector<PageSection>(1);//NOLINT(misc-non-private-member-variables-in-classes)
			[[nodiscard ]] bool IsPageContiguous()const noexcept;
			[[nodiscard ]] uint64_t GetFrreBytesInPage()const noexcept;
			[[nodiscard ]] std::pair<uint64_t/*in page offset*/, uint64_t/*size Bytes*/> GetNextFreeSection()const noexcept;
			void SortSections() noexcept;
			void PruneEmptySections();
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
		[[nodiscard]] bool MapResetRom(std::shared_ptr<IO_DEVICES::DeviceBase> device, uint64_t sizeBytes, uint64_t resetVector);
		[[nodiscard]] bool MapFirmwareRom(std::shared_ptr<IO_DEVICES::DeviceBase> device, uint64_t sizeBytes, uint64_t FirmwareEntry);
		[[nodiscard]] bool MapMainMemory(std::shared_ptr<IO_DEVICES::DeviceBase> device, uint64_t sizeBytes, uint64_t Base);
		[[nodiscard]] bool MapIODevice(std::shared_ptr<IO_DEVICES::DeviceBase> device) noexcept;

		[[nodiscard]] uint8_t Read8(uint64_t address) const noexcept;
		[[nodiscard]] uint16_t Read16(uint64_t address) const noexcept;
		[[nodiscard]] uint32_t Read32(uint64_t address) const noexcept;
		[[nodiscard]] uint64_t Read64(uint64_t address) const noexcept;
		void Write8(uint64_t address, uint8_t value) noexcept;
		void Write16(uint64_t address, uint16_t value) noexcept;
		void Write32(uint64_t address, uint32_t value) noexcept;
		void Write64(uint64_t address, uint64_t value) noexcept;
	

		void PrintMemoryMap() const noexcept;
		void DumpMemoryToStdout() const noexcept;
	};
}// namespace X86_64_EMU_SOFT::SYSTEM::MEMORY