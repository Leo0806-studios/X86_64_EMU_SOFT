#include <algorithm>
#include "SYSTEM/MEMORY/MEMORY.h"

bool X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::RegisterIODevice(std::shared_ptr<IO_DEVICEs::DeviceBase> device, uint64_t sizeBytes, uint64_t preferedBaseAdress)
{
	DeviceInfos newDeviceInfo{ .device =device,.sizeBytes = sizeBytes,.baseAdress=preferedBaseAdress };
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
	const uint64_t TotalAddressSpacePages = ((TotalAddresSpaceBytes / 1024 ) + 4095) / 4096;//we round up to the next page
	MemoryPages.resize(TotalAddressSpacePages);

	for (uint64_t deviceIndex = 1/*we skip main memory for now*/; deviceIndex < RegisteredDevices.size(); deviceIndex++) {
		uint64_t RemainingDeviceBytes = RegisteredDevices[deviceIndex].sizeBytes;
		const uint64_t PreferedBasePage = GetPageNumber(RegisteredDevices[deviceIndex].baseAdress);
		//now we forward scan from the prefered base to see if all would fit
		uint64_t FreePagesCount = 0;
		for (uint64_t page = PreferedBasePage; page < MemoryPages.size(); page++) {
		}

	}


	//now we map the main memory to all remainging free pages starting from 0
	uint64_t offset = 0;
	for ( auto& PageEntry : MemoryPages) {
		if(PageEntry.device == nullptr){

			PageEntry.device = RegisteredDevices[0].device.get();
			PageEntry.pageOffset = offset;
			offset++;
		}
	}
	return true;
}

void X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::PrintMemoryMap() const noexcept
{
	for (const auto& deviceInfo : RegisteredDevices) {
		std::print("Device: {}, Base Address: {}, Size: {}KB\n", typeid(*deviceInfo.device).name(), deviceInfo.baseAdress, deviceInfo.pageCount * 4);
	}
	for (const auto& page : MemoryPages) {
		if (page.device) {
			std::print("Page mapped to device: {}, page offset: {}\n", typeid(*page.device).name(), page.pageOffset);
		}
		else {
			std::print("Page not mapped to any device\n");
		}
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
	uint64_t usedBytes = 0;
	for (const auto& section : Sections) {
		usedBytes += section.size;
	}
	return 4096 - usedBytes;
}

std::pair<uint64_t, uint64_t> X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus::PageEntry::GetNextFreeSection() const noexcept
{
	for(uint64_t SectionIndex=0;SectionIndex<Sections.size();SectionIndex++){
		const auto& thisSection = Sections[SectionIndex];
		if (SectionIndex == Sections.size() - 1) {
			//last section handeling
			const uint32_t freeBytes = 4096-(thisSection.pageOffset + thisSection.size);
			if(freeBytes >0){
				return { thisSection.pageOffset + thisSection.size, freeBytes };
			}
			else {
				return { 0,0 };
			}
		}
		const auto& nextSection = Sections[SectionIndex + 1];
		const uint32_t freeBytes = nextSection.pageOffset - (thisSection.pageOffset + thisSection.size);
		if (freeBytes > 0) {
			return { thisSection.pageOffset + thisSection.size, freeBytes };
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
