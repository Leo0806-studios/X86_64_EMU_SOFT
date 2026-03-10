#include <algorithm>
#include "SYSTEM/MEMORY/MEMORY.h"

bool X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::RegisterIODevice(std::shared_ptr<IO_DEVICES::DeviceBase> device, uint64_t sizeBytes, uint64_t preferedBaseAdress)
{
	DeviceInfos newDeviceInfo{ .device = device,.sizeBytes = sizeBytes,.baseAdress = preferedBaseAdress };
	RegisteredDevices.push_back(newDeviceInfo);
	return true;
}
namespace {
	uint64_t GetPageNumber(uint64_t address) {
		return address >> 12;
	}





}

bool X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::BuildPageTable() noexcept
{
	std::sort(RegisteredDevices.begin(), RegisteredDevices.end(), [](const DeviceInfos& a, const DeviceInfos& b) {
		return a.baseAdress < b.baseAdress;
			  });

	//we now know that the first element must be the MEMORY device since its the only one allowed to have a preferred base address of 0.
	uint64_t TotalAddresSpaceBytes = 0;
	for (const auto& deviceInfo : RegisteredDevices)
	{
		TotalAddresSpaceBytes += deviceInfo.sizeBytes;
	}
	const uint64_t TotalAddressSpacePages = ((TotalAddresSpaceBytes)+4095) / 4096;//we round up to the next page
	MemoryPages.resize(TotalAddressSpacePages);
	for (auto& page : MemoryPages) {
		page.SortSections();
	}
	for (uint64_t deviceIndex = 1/*we skip main memory for now*/; deviceIndex < RegisteredDevices.size(); deviceIndex++) {
		const uint64_t PreferedBasePage = GetPageNumber(RegisteredDevices[deviceIndex].baseAdress);
		const auto& currentDevice = RegisteredDevices[deviceIndex];
		//now we forward scan from the prefered base to see if all would fit
		uint64_t StartPAgeOfFreeRegion = 0;
		uint32_t inPageOffsetOfFirstFreePage = 0;
		uint64_t FreeBytes = 0;
		bool foundSpace = false;
		for (uint64_t page = PreferedBasePage; page < MemoryPages.size(); page++) {
			PageEntry& currentPage = MemoryPages[page];
			for (const auto& section : currentPage.Sections) {
				if (FreeBytes >= currentDevice.sizeBytes) {
					foundSpace = true;
					break;
				}
				if (section.device == nullptr) {
					if (FreeBytes == 0) {
						StartPAgeOfFreeRegion = page;
						inPageOffsetOfFirstFreePage = section.pageOffset;
					}
					FreeBytes += section.size;
				}
				else {
					FreeBytes = 0;
				}
			}
			if (foundSpace) {
				break;
			}


		if (!foundSpace) {
			break;
		}
		}
		uint64_t remainingDeviceBytes = currentDevice.sizeBytes;
		uint64_t deviceOffset = 0;
		for (uint64_t pageIndex = StartPAgeOfFreeRegion; pageIndex< MemoryPages.size();pageIndex++) {
			auto& currentPage = MemoryPages[pageIndex];
			currentPage.SortSections();
			if (pageIndex == StartPAgeOfFreeRegion) {
				for (auto& section : currentPage.Sections) {

					if (section.size <= remainingDeviceBytes) {

					section.device = currentDevice.device.get();
					section.DeviceOffset = deviceOffset;
					deviceOffset += section.size;
					remainingDeviceBytes -= section.size;
					}
					else {
						uint64_t newSize = section.size - remainingDeviceBytes;
						uint64_t newOffset = section.pageOffset + remainingDeviceBytes;
						PageEntry::PageSection newSection{ .device = nullptr,.DeviceOffset = 0,.pageOffset = static_cast<uint32_t>(newOffset),.size = static_cast<uint32_t>(newSize) };
						section.device = currentDevice.device.get();
						section.DeviceOffset = deviceOffset;
						section.size = static_cast<uint32_t>(remainingDeviceBytes);
						remainingDeviceBytes -= section.size;
						deviceOffset += section.size;
						currentPage.Sections.push_back(newSection);//should be ok as i push at the ned of the sectionvector
						break;
					}
					
				}
				continue;
			}
		}
	}

	//now we map the main memory to all remainging free pages starting from 0
	uint64_t offset = 0;
	for (auto& PageEntry : MemoryPages) {
		for (auto& section : PageEntry.Sections) {
			if (section.device == nullptr) {

			section.device = RegisteredDevices[0].device.get();
			section.DeviceOffset = offset;
			offset += section.size;
			}
		}

	}
	return true;
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
