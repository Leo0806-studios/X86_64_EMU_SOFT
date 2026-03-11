#include <algorithm>
#include "SYSTEM/MEMORY/MEMORY.h"

#include "SYSTEM/IO_DEVICES/MAIN_MEMORY_DEVICE.h"
namespace {
	uint64_t GetPageNumber(uint64_t address) {
		return address >> 12;
	}



	

}



void X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::PrintMemoryMap() const noexcept
{
	for (const auto& deviceInfo : RegisteredDevices) {
		std::print("Device: {}, Base Address: {}, Size: {}KB\n", typeid(*deviceInfo.device).name(), deviceInfo.baseAdress, deviceInfo.sizeBytes / 1024);
	}
	uint64_t pagenum = 0;
	for (const auto& page : MemoryPages) {
		std::print("Page : {}\n", pagenum);
		for (const auto& section : page.Sections) {

			if (section.device) {
				std::print("Page Section from pageoffset {} to {} mapped to device: {}, Device Offset: {}\n", section.pageOffset, section.pageOffset + section.size, typeid(*section.device).name(), section.DeviceOffset);
			}
			else {
				std::print("Page section  not mapped to any device\n");
			}
		}
		pagenum++;
	}
}

bool X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::PageEntry::IsPageContiguous() const noexcept
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

uint64_t X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::PageEntry::GetFrreBytesInPage() const noexcept
{
	uint64_t FreeBytes = 0;
	for (const auto& section : Sections) {
		if (section.device == nullptr) {
			FreeBytes += section.size;
		}
	}
	return FreeBytes;
}

std::pair<uint64_t, uint64_t> X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::PageEntry::GetNextFreeSection() const noexcept
{
	for (const auto& section : Sections) {
		if (section.device == nullptr) {
			return { section.pageOffset, section.size };
		}
	}
	return { 0,0 };
}

void X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::PageEntry::SortSections() noexcept
{
	std::sort(Sections.begin(), Sections.end(), [](const PageSection& a, const PageSection& b) {
		return a.pageOffset < b.pageOffset;
			  });
}

void X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::PageEntry::PruneEmptySections()
{
	for (auto section = Sections.rbegin(); section != Sections.rend(); section++) {
		if ((*section).size == 0) {
			Sections.erase(section.base());

		}
	}
}

bool X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::MapResetRom(std::shared_ptr<IO_DEVICES::DeviceBase> device, uint64_t sizeBytes, uint64_t resetVector)
{
	const uint64_t resetStartPage = GetPageNumber(resetVector);
	const uint64_t reserEndPage = GetPageNumber(resetVector + sizeBytes);
	const uint64_t inPageOffet = resetVector & 0xFFFULL;
	DeviceInfos info{ .device = device,.sizeBytes = sizeBytes,.baseAdress = resetVector };
	RegisteredDevices.push_back(info);

	//we can assume that only one section per page exists since thats the default amount and this device is the first one that gets mapped
	for (uint64_t page = resetStartPage; page <= reserEndPage; page++) {
		PageEntry& currentPage = MemoryPages[page];
		if (inPageOffet == 0) {

			PageEntry::PageSection newSection{ .device = currentPage.Sections[0].device,.DeviceOffset = 0,.pageOffset = static_cast<uint32_t>(sizeBytes+1),.size = static_cast<uint32_t>(4096 - sizeBytes) };
			currentPage.Sections[0].device = device.get();
			currentPage.Sections[0].DeviceOffset = 0;
			currentPage.Sections[0].pageOffset = 0;
			currentPage.Sections[0].size = static_cast<uint32_t>(sizeBytes);
			currentPage.Sections.push_back(newSection);
			currentPage.SortSections();



		}
		else if (inPageOffet + sizeBytes == 4096) {
			PageEntry::PageSection newSection{ .device = currentPage.Sections[0].device,.DeviceOffset = 0,.pageOffset = 0,.size = static_cast<uint32_t>(4096 - sizeBytes) };
			currentPage.Sections[0].device = device.get();
			currentPage.Sections[0].DeviceOffset = 0;
			currentPage.Sections[0].pageOffset = static_cast<uint32_t>(inPageOffet);
			currentPage.Sections[0].size = static_cast<uint32_t>(sizeBytes);
			currentPage.Sections.push_back(newSection);
			currentPage.SortSections();

		}
		else{




			PageEntry::PageSection newSectionBefore{ .device = currentPage.Sections[0].device,.DeviceOffset = 0,.pageOffset = 0,.size = static_cast<uint32_t>(4096ULL - inPageOffet) };
			PageEntry::PageSection newSectionAfter{ .device = currentPage.Sections[0].device,.DeviceOffset = 0,.pageOffset = static_cast<uint32_t>(inPageOffet + sizeBytes+1),.size = static_cast<uint32_t>(4096ULL - (inPageOffet + sizeBytes)) };
			currentPage.Sections[0].device = device.get();
			currentPage.Sections[0].DeviceOffset = 0;
			currentPage.Sections[0].pageOffset = static_cast<uint32_t>(inPageOffet);
			currentPage.Sections[0].size = static_cast<uint32_t>(sizeBytes);
			currentPage.Sections.push_back(newSectionBefore);
			currentPage.Sections.push_back(newSectionAfter);
			currentPage.SortSections();
		}
		currentPage.PruneEmptySections();
	}
	
	return true;
}

bool X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::MapFirmwareRom(std::shared_ptr<IO_DEVICES::DeviceBase> device, uint64_t sizeBytes, uint64_t FirmwareEntry)
{
	const uint64_t firmwareStartPage = GetPageNumber(FirmwareEntry);
	const uint64_t inPageOffet = FirmwareEntry & 0xFFFULL;
	DeviceInfos info{ .device = device,.sizeBytes = sizeBytes,.baseAdress = FirmwareEntry };
	uint64_t remainingDeviceBytes = sizeBytes;
	RegisteredDevices.push_back(info);
	for (uint64_t page = firmwareStartPage; page < MemoryPages.size() ; page++) {
		PageEntry& currentPage = MemoryPages[page];
		if (page == firmwareStartPage) {
			for (auto& section : currentPage.Sections) {
				if (section.pageOffset < inPageOffet) {
					continue;
				}
				//we can assume that a page has at most two sections: a MainMemory section and a ResetRom section


				if (typeid(*section.device) != typeid(IO_DEVICES::MainMemoryDevice)) {
					return false;
				}
				if (section.size >= remainingDeviceBytes) {
					PageEntry::PageSection newSections = {};


					//PageEntry::PageSection newSectionBefore{ .device = section.device,.DeviceOffset = 0,.pageOffset = 0,.size = static_cast<uint32_t>(4096ULL - inPageOffet) };
					PageEntry::PageSection newSectionAfter{ .device = section.device,.DeviceOffset = 0,.pageOffset = static_cast<uint32_t>(inPageOffet + sizeBytes+1),.size = static_cast<uint32_t>(4096ULL - (inPageOffet + sizeBytes)) };
					section.device = device.get();
					section.DeviceOffset = 0;
					section.pageOffset = static_cast<uint32_t>(inPageOffet);
					section.size = static_cast<uint32_t>(sizeBytes);
					//currentPage.Sections.push_back(newSectionBefore);
					currentPage.Sections.push_back(newSectionAfter);

				}

			}
			currentPage.SortSections();
			continue;
		}
		currentPage.PruneEmptySections();
	}
	return true;
}

bool X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::MapMainMemory(std::shared_ptr<IO_DEVICES::DeviceBase> device, uint64_t sizeBytes, uint64_t Base)
{
	uint64_t amountPages = (sizeBytes + 4095) / 4096;
	uint64_t deviceOffset = 0;
	std::ignore = Base;
	MemoryPages.resize(amountPages);
	for (auto& page : MemoryPages) {
		page.Sections.resize(1);
		page.Sections[0].device = device.get();
		page.Sections[0].pageOffset = 0;
		page.Sections[0].DeviceOffset = deviceOffset;
		page.Sections[0].size = 4096;
		deviceOffset += page.Sections[0].size;
	}
	return true;
}

bool X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::MapIODevice(std::shared_ptr<IO_DEVICES::DeviceBase> device) noexcept
{
	std::ignore = device;
	return false;
}

uint8_t X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::Read8(uint64_t address) const noexcept
{
	const uint64_t PageNumber(address);
	const uint64_t inPageOffset = address & 0xFFFULL;
	auto& page = MemoryPages[PageNumber];
	for (auto& section : page.Sections) {
		if (inPageOffset >= section.pageOffset && inPageOffset < (section.pageOffset + section.size)) {
			const uint64_t off = inPageOffset - section.pageOffset;
			return  section.device->Read8(section.DeviceOffset + off);
		}
	}
	return 0xFF;
}

uint16_t X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::Read16(uint64_t address) const noexcept
{
	uint16_t value = 0;
	value |= Read8(address);
	value |= static_cast<uint16_t>(Read8(address + 1)) << 8;
	return value;
}

uint32_t X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::Read32(uint64_t address) const noexcept
{
	uint32_t value = 0;
	value |= Read8(address);
	value |= static_cast<uint32_t>(Read8(address+ 1)) << 8;
	value |= static_cast<uint32_t>(Read8(address+ 2)) << 16;
	value |= static_cast<uint32_t>(Read8(address+ 3)) << 24;
	return value;
}

uint64_t X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::Read64(uint64_t address) const noexcept
{
	uint64_t value = 0;
	value |= Read8(address);
	value |= static_cast<uint64_t>(Read8(address+ 1)) << 8;
	value |= static_cast<uint64_t>(Read8(address+ 2)) << 16;
	value |= static_cast<uint64_t>(Read8(address+ 3)) << 24;
	value |= static_cast<uint64_t>(Read8(address+ 4)) << 32;
	value |= static_cast<uint64_t>(Read8(address+ 5)) << 40;
	value |= static_cast<uint64_t>(Read8(address+ 6)) << 48;
	value |= static_cast<uint64_t>(Read8(address+ 7)) << 56;
	return value;
}

void X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::Write8(uint64_t address, uint8_t value) noexcept
{
	const uint64_t PageNumber(address);
	const uint64_t inPageOffset = address & 0xFFFULL;
	auto& page = MemoryPages[PageNumber];
	for (auto& section : page.Sections) {
		if (inPageOffset >= section.pageOffset && inPageOffset < (section.pageOffset + section.size)) {
			const uint64_t off = inPageOffset - section.pageOffset;
			  section.device->Write8(section.DeviceOffset + off,value);
		}
	}
}

void X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::Write16(uint64_t address, uint16_t value) noexcept
{
	Write8(address, value & 0xFFULL) ;
	Write8(address, (value >> 8) & 0xFFULL);
}

void X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::Write32(uint64_t address, uint32_t value) noexcept
{
	Write8(address,(value>>0)&0xFFULL);
	Write8(address,(value>>8)&0xFFULL);
	Write8(address,(value>>16)&0xFFULL);
}

void X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::Write64(uint64_t address, uint64_t value) noexcept
{
	Write8(address,(value>>0)&0xFFULL);
	Write8(address,(value>>8)&0xFFULL);
	Write8(address,(value>>16)&0xFFULL);
	Write8(address,(value>>24)&0xFFULL);
	Write8(address,(value>>32)&0xFFULL);
	Write8(address,(value>>40)&0xFFULL);
	Write8(address,(value>>48)&0xFFULL);
	Write8(address,(value>>56)&0xFFULL);

}
