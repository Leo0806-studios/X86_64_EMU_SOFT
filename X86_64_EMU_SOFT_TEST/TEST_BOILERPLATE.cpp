#include "TEST_BOILERPLATE.h"
#include <SYSTEM/IO_DEVICES/MAIN_MEMORY_DEVICE.h>

namespace X86_64_EMU_SOFT::TESTS {

	std::unique_ptr<SYSTEM::CPU::VirtualCore> X86_64_EMU_SOFT::TESTS::Boilerplate::CreateVirtualCoreWithZeroRam(uint64_t maxRamKB,SYSTEM::CPU::vCoreMode startupMode)
	{
		std::shared_ptr<SYSTEM::IO_DEVICES::MainMemoryDevice> mainMemoryDevice = std::make_shared<SYSTEM::IO_DEVICES::MainMemoryDevice>(maxRamKB);
		for (uint64_t i = 0; i < maxRamKB ; i++) {
			mainMemoryDevice->Write8(i, 0);
		}
		std::shared_ptr<SYSTEM::MEMORY::MemoryBus> memoryBus = CreateMemBus();
		memoryBus->MapMainMemory(mainMemoryDevice, maxRamKB*1024, 0);
		return std::make_unique<SYSTEM::CPU::VirtualCore>(0, memoryBus, startupMode);


	}
	std::unique_ptr<SYSTEM::CPU::VirtualCore> Boilerplate::CreateVirtualCore(uint64_t resetVector, std::shared_ptr<SYSTEM::MEMORY::MemoryBus> memoryBus, SYSTEM::CPU::vCoreMode startupMode)
	{
		return std::make_unique<SYSTEM::CPU::VirtualCore>(resetVector, std::move(memoryBus), startupMode);
	}
	std::unique_ptr<SYSTEM::MEMORY::MemoryBus> Boilerplate::CreateMemBus()
	{
		return std::make_unique<SYSTEM::MEMORY::MemoryBus>();
	}
	std::shared_ptr<SYSTEM::IO_DEVICES::MainMemoryDevice> Boilerplate::CreateMainMemoryDevice(uint64_t maxRamKB)
	{
		return std::make_shared<SYSTEM::IO_DEVICES::MainMemoryDevice>(maxRamKB);
	}
	std::shared_ptr<SYSTEM::IO_DEVICES::MainMemoryDevice> Boilerplate::CreateMainMemoryDevicePreFilled(const std::vector<uint8_t>& bytes)
	{
		uint64_t sizeKB = (bytes.size() + 1023) / 1024;
		auto a = std::make_shared<SYSTEM::IO_DEVICES::MainMemoryDevice>(sizeKB);

		for (uint64_t i = 0; i < bytes.size(); i++) {
			a->Write8(i, bytes[i]);
		}
		return a;
	}
}
