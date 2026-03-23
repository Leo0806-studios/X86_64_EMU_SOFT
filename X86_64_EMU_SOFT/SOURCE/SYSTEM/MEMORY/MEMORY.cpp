#include <algorithm>

#include <array>
#include <cstdint>
#include <HELPERS/MACROS.h>
#include <HELPERS/MACROS.h>
#include <intrin.h>
#include <memory>
#include <print>
#include <tracy/Tracy.hpp>
#include <tuple>
#include <utility>
#include "HELPERS/REDEFINE_MACROS.h"
#include "SYSTEM/IO_DEVICES/DEVICE_BASE.h"
#include "SYSTEM/IO_DEVICES/MAIN_MEMORY_DEVICE.h"
#include "SYSTEM/MEMORY/MEMORY.h"
#include <SYSTEM/IO_DEVICES/RESET_ROM.h>
#include <SYSTEM/IO_DEVICES/FIRMWARE.h>
namespace {
	[[nodiscard]] uint64_t GetPageNumber(uint64_t address) {
		return address >> 12U;
	}
}// namespace


namespace X86_64_EMU_SOFT::SYSTEM::MEMORY {

	void  MemoryBus::PrintMemoryMap() const noexcept//NOLINT(bugprone-exception-escape)
	{
		ZoneNamed(PrintMemoryMap, true);
		for (const auto& deviceInfo : RegisteredDevices) {
			if (deviceInfo.sizeBytes < 1024) {
				std::print("Device: {}, Base Address: {}, Size: {}B\n", typeid(*deviceInfo.device).name(), deviceInfo.baseAdress, deviceInfo.sizeBytes);//NOSONAR //NOLINT(clang-diagnostic-potentially-evaluated-expression)
			}
			else
			{

				std::print("Device: {}, Base Address: {}, Size: {}KB\n", typeid(*deviceInfo.device).name(), deviceInfo.baseAdress, deviceInfo.sizeBytes / 1024);//NOSONAR //NOLINT(clang-diagnostic-potentially-evaluated-expression)
			}
		}
		uint64_t pagenum = 0;
		for (const auto& page : MemoryPages) {
			std::print("Page : {}\n", pagenum);
			for (const auto& section : page.Sections) {

				if (section.device) {
					std::print("Page Section from pageoffset {} to {} mapped to device: {}, Device Offset: {}\n", section.pageOffset, section.pageOffset + (section.size - 1), typeid(*section.device).name(), section.DeviceOffset);
				}
				else {
					std::print("Page section  not mapped to any device\n");
				}
			}
			pagenum++;
		}
	}

	void  MemoryBus::DumpMemoryToStdout() const noexcept//NOLINT(bugprone-exception-escape)
	{
		ZoneNamed(DumpMemoryToStdout, true);

		for (const auto& page : MemoryPages) {
			for (const auto& section : page.Sections) {
				if (section.device) {
					for (uint64_t i = 0; i < section.size; i++) {//NOSONAR
						uint8_t value = section.device->Read8(section.DeviceOffset + i);
						std::print("{:02X} ", value);
					}
				}
				else {
					for (uint64_t i = 0; i < section.size; i++) {//NOSONAR
						std::print("?? ");
					}
				}
			}
			std::print("\n");
		}

	}

	bool  MemoryBus::PageEntry::IsPageContiguous() const noexcept
	{
		if (Sections.empty()) {
			return true;
		}
		uint64_t expectedOffset = 0;
		for (const auto& section : Sections) {
			if (section.pageOffset != expectedOffset) {
				return false;
			}
			expectedOffset += section.size;
		}
		return true;

	}

	uint64_t  MemoryBus::PageEntry::GetFrreBytesInPage() const noexcept
	{
		uint64_t FreeBytes = 0;
		for (const auto& section : Sections) {
			if (section.device == nullptr) {
				FreeBytes += section.size;
			}
		}
		return FreeBytes;
	}

	std::pair<uint64_t, uint64_t>  MemoryBus::PageEntry::GetNextFreeSection() const noexcept
	{
		for (const auto& section : Sections) {
			if (section.device == nullptr) {
				return { section.pageOffset, section.size };
			}
		}
		return { 0,0 };
	}

	void  MemoryBus::PageEntry::SortSections() noexcept
	{
		std::sort(Sections.begin(), Sections.end(), [](const PageSection& a, const PageSection& b) {
			return a.pageOffset < b.pageOffset;
				  });
	}

	void  MemoryBus::PageEntry::PruneEmptySections()
	{
		for (auto section = Sections.rbegin(); section != Sections.rend(); section++) {
			if ((*section).size == 0) {
				Sections.erase(section.base());

			}
		}
	}

	bool  MemoryBus::MapResetRom(std::shared_ptr<IO_DEVICES::DeviceBase> device, uint64_t sizeBytes, uint64_t resetVector)//NOLINT(performance-unnecessary-value-param)
	{
		ZoneScoped;

		const uint64_t resetStartPage = GetPageNumber(resetVector);
		const uint64_t reserEndPage = GetPageNumber(resetVector + sizeBytes);
		const uint64_t inPageOffet = resetVector & 0xFFFULL;
		DeviceInfos info{ .device = device,.sizeBytes = sizeBytes,.baseAdress = resetVector };
		RegisteredDevices.push_back(std::move(info));

		//we can assume that only one section per page exists since thats the default amount and this device is the first one that gets mapped
		for (uint64_t page = resetStartPage; page <= reserEndPage; page++) {
			PageEntry& currentPage = MemoryPages[page];
			if (inPageOffet == 0) {

				const PageEntry::PageSection newSection{ .device = currentPage.Sections[0].device,.DeviceOffset = 0,.pageOffset = static_cast<uint32_t>(sizeBytes),.size = static_cast<uint32_t>(4096 - sizeBytes) };
				currentPage.Sections[0].device = device.get();
				currentPage.Sections[0].DeviceOffset = 0;
				currentPage.Sections[0].pageOffset = 0;
				currentPage.Sections[0].size = static_cast<uint32_t>(sizeBytes);
				currentPage.Sections[0].deviceTag = PageEntry::PageSection::DeviceTag::ResetRom;
				currentPage.Sections.push_back(newSection);
				currentPage.SortSections();



			}
			else if (inPageOffet + sizeBytes == 4096) {
				const PageEntry::PageSection newSection{ .device = currentPage.Sections[0].device,.DeviceOffset = 0,.pageOffset = 0,.size = static_cast<uint32_t>(4096 - sizeBytes) };
				currentPage.Sections[0].device = device.get();
				currentPage.Sections[0].DeviceOffset = 0;
				currentPage.Sections[0].pageOffset = static_cast<uint32_t>(inPageOffet);
				currentPage.Sections[0].size = static_cast<uint32_t>(sizeBytes);
				currentPage.Sections[0].deviceTag = PageEntry::PageSection::DeviceTag::ResetRom;
				currentPage.Sections.push_back(newSection);
				currentPage.SortSections();

			}
			else {




				const PageEntry::PageSection newSectionBefore{ .device = currentPage.Sections[0].device,.DeviceOffset = 0,.pageOffset = 0,.size = static_cast<uint32_t>(4096ULL - inPageOffet) };
				const PageEntry::PageSection newSectionAfter{ .device = currentPage.Sections[0].device,.DeviceOffset = 0,.pageOffset = static_cast<uint32_t>(inPageOffet + sizeBytes + 1),.size = static_cast<uint32_t>(4096ULL - (inPageOffet + sizeBytes)) };
				currentPage.Sections[0].device = device.get();
				currentPage.Sections[0].DeviceOffset = 0;
				currentPage.Sections[0].pageOffset = static_cast<uint32_t>(inPageOffet);
				currentPage.Sections[0].size = static_cast<uint32_t>(sizeBytes);
				currentPage.Sections[0].deviceTag = PageEntry::PageSection::DeviceTag::ResetRom;
				currentPage.Sections.push_back(newSectionBefore);
				currentPage.Sections.push_back(newSectionAfter);
				currentPage.SortSections();
			}
			currentPage.PruneEmptySections();
		}

		return true;
	}

	bool  MemoryBus::MapFirmwareRom(std::shared_ptr<IO_DEVICES::DeviceBase> device, uint64_t sizeBytes, uint64_t FirmwareEntry)//NOLINT(performance-unnecessary-value-param)
	{
		ZoneScoped;
		const uint64_t firmwareStartPage = GetPageNumber(FirmwareEntry);
		const uint64_t inPageOffet = FirmwareEntry & 0xFFFULL;
		const DeviceInfos info{ .device = device,.sizeBytes = sizeBytes,.baseAdress = FirmwareEntry };
		uint64_t remainingDeviceBytes = sizeBytes;
		RegisteredDevices.push_back(std::move(info));
		for (uint64_t page = firmwareStartPage; page < MemoryPages.size(); page++) {
			PageEntry& currentPage = MemoryPages[page];
			if (page == firmwareStartPage) {
				for (auto& section : currentPage.Sections) {
					if (section.pageOffset < inPageOffet) {
						continue;
					}
					//we can assume that a page has at most two sections: a MainMemory section and a ResetRom section


					if (typeid(*section.device) != typeid(IO_DEVICES::MainMemoryDevice)) {//NOSONAR

						return false;
					}
					if (section.size >= remainingDeviceBytes) {//NOSONAR


						//PageEntry::PageSection newSectionBefore{ .device = section.device,.DeviceOffset = 0,.pageOffset = 0,.size = static_cast<uint32_t>(4096ULL - inPageOffet) };
						const PageEntry::PageSection newSectionAfter{ .device = section.device,.DeviceOffset = 0,.pageOffset = static_cast<uint32_t>(inPageOffet + sizeBytes),.size = static_cast<uint32_t>(4096ULL - (inPageOffet + sizeBytes)) };
						section.device = device.get();
						section.DeviceOffset = 0;
						section.pageOffset = static_cast<uint32_t>(inPageOffet);
						section.size = static_cast<uint32_t>(sizeBytes);
						section.deviceTag = PageEntry::PageSection::DeviceTag::FirmwareRom;
						//currentPage.Sections.push_back(newSectionBefore);
						currentPage.Sections.push_back(newSectionAfter);
						remainingDeviceBytes = 0;

					}
					else {
						__debugbreak();
					}

				}
				currentPage.SortSections();
				continue;
			}
			currentPage.PruneEmptySections();
		}
		return true;
	}

	bool  MemoryBus::MapMainMemory(std::shared_ptr<IO_DEVICES::DeviceBase> device, uint64_t sizeBytes, uint64_t Base)
	{
		ZoneNamed(MapMainMemory, true);
		const uint64_t amountPages = (sizeBytes + 4095) / 4096;
		uint64_t deviceOffset = 0;
		DeviceInfos info{ .device = device, .sizeBytes = sizeBytes,.baseAdress = Base };
		RegisteredDevices.push_back(std::move(info));
		std::ignore = Base;
		MemoryPages.resize(amountPages);
		for (auto& page : MemoryPages) {
			page.Sections.resize(1);
			page.Sections[0].device = device.get();
			page.Sections[0].pageOffset = 0;
			page.Sections[0].DeviceOffset = deviceOffset;
			page.Sections[0].size = 4096;
			page.Sections[0].deviceTag = PageEntry::PageSection::DeviceTag::MainMemory;
			deviceOffset += page.Sections[0].size;
		}
		return true;
	}

	bool  MemoryBus::MapIODevice(std::shared_ptr<IO_DEVICES::DeviceBase> device) noexcept//NOLINT(performance-unnecessary-value-param)
	{
		std::ignore = device;
		return false;
	}

	uint8_t  MemoryBus::Read8(uint64_t address) const noexcept
	{

		const uint64_t PageNumber = address >> 12ULL;
		const uint64_t inPageOffset = address & 0xFFFULL;
		const auto& page = MemoryPages[PageNumber];
		for (const auto& section : page.Sections) {
			if (inPageOffset >= section.pageOffset && inPageOffset < (section.pageOffset + section.size)) {
				const uint64_t off = inPageOffset - section.pageOffset;
				switch (section.deviceTag) {
					case PageEntry::PageSection::DeviceTag::MainMemory: {
						return static_cast<IO_DEVICES::MainMemoryDevice*>(section.device)->Read8(section.DeviceOffset + off);
					}
					case PageEntry::PageSection::DeviceTag::ResetRom: {
						return static_cast<IO_DEVICES::ResetROMDevice*>(section.device)->Read8(section.DeviceOffset + off);
					}
					case PageEntry::PageSection::DeviceTag::FirmwareRom: {
						return static_cast<IO_DEVICES::FirmwareRomDevice*>(section.device)->Read8(section.DeviceOffset + off);
					}
					case PageEntry::PageSection::DeviceTag::OtherDevice:
					default: {
						return section.device->Read8(section.DeviceOffset + off);
					}
				}
			}
		}
		return 0xFF;
	}

	uint16_t  MemoryBus::Read16(uint64_t address) const noexcept
	{
		uint16_t value = 0;
		std::array<uint8_t, sizeof(value)> arr = { Read8(address),Read8(address + 1) };
		static_assert(sizeof(value) == sizeof(arr), "value and tmp buffer are not the same size (uint16_t)");
		std::memcpy(&value, arr.data(), sizeof(value));
		return value;
	}

	uint32_t  MemoryBus::Read32(uint64_t address) const noexcept
	{
		uint32_t value = 0;
		std::array<uint8_t, sizeof(value)> arr = { Read8(address + 0U),Read8(8 + 1U),Read8(address + 2U),Read8(address + 3U) };
		static_assert(sizeof(value) == sizeof(arr), "value and tmp buffer are not the same size (uint32_t)");
		std::memcpy(&value, arr.data(), sizeof(value));
		return value;
	}

	uint64_t  MemoryBus::Read64(uint64_t address) const noexcept
	{
		uint64_t value = 0;
		std::array<uint8_t, sizeof(value)> arr = { Read8(address + 0U),Read8(address + 1U),Read8(address + 2U),Read8(address + 3U),
													Read8(address + 4U),Read8(address + 5U),Read8(address + 6U),Read8(address + 7U) };
		static_assert(sizeof(value) == sizeof(arr), "value and tmp buffer are not the same size (uint32_t)");
		std::memcpy(&value, arr.data(), sizeof(value));
		return value;

	}

	void  MemoryBus::Write8(uint64_t address, uint8_t value) noexcept
	{
		const uint64_t PageNumber(address);
		const uint64_t inPageOffset = address & 0xFFFULL;
		auto& page = MemoryPages[PageNumber];
		for (auto& section : page.Sections) {
			if (inPageOffset >= section.pageOffset && inPageOffset < (section.pageOffset + section.size)) {
				const uint64_t off = inPageOffset - section.pageOffset;
				section.device->Write8(section.DeviceOffset + off, value);
			}
		}
	}

	void  MemoryBus::Write16(uint64_t address, uint16_t value) noexcept
	{
		std::array<uint8_t, sizeof(value)> arr{ 0,0 };
		std::memcpy(arr.data(), &value, sizeof(arr));
		for (const auto& byte : arr) {
			Write8(address, byte);
			address++;
		}
	}

	void  MemoryBus::Write32(uint64_t address, uint32_t value) noexcept
	{
		std::array<uint8_t, sizeof(value)> arr{ 0,0 };
		std::memcpy(arr.data(), &value, sizeof(arr));
		for (const auto& byte : arr) {
			Write8(address, byte);
			address++;
		}
	}

	void  MemoryBus::Write64(uint64_t address, uint64_t value) noexcept
	{
		std::array<uint8_t, sizeof(value)> arr{ 0,0 };
		std::memcpy(arr.data(), &value, sizeof(arr));
		for (const auto& byte : arr) {
			Write8(address, byte);
			address++;
		}

	}

}// namespace X86_64_EMU_SOFT::SYSTEM::MEMORY