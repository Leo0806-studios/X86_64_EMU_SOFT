#include "TEST_BOILERPLATE.h"
#include <SYSTEM/CPU/VCORE.h>
#include <SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h>
#include <SYSTEM/CPU/REGISTERS/GPR.h>
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
		using namespace SYSTEM::CPU;
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
	inline void Boilerplate::SetRegistersAscending(std::shared_ptr<SYSTEM::CPU::VirtualCore>& vCore) noexcept {
		using namespace SYSTEM::CPU;
		static_cast<REGISTERS::GPR&>(vCore->GetRegister(INSTRUCTIONS::TargetRegister::RAX)).SetValue(1);
		static_cast<REGISTERS::GPR&>(vCore->GetRegister(INSTRUCTIONS::TargetRegister::RBX)).SetValue(2);
		static_cast<REGISTERS::GPR&>(vCore->GetRegister(INSTRUCTIONS::TargetRegister::RCX)).SetValue(3);
		static_cast<REGISTERS::GPR&>(vCore->GetRegister(INSTRUCTIONS::TargetRegister::RDX)).SetValue(4);
		static_cast<REGISTERS::GPR&>(vCore->GetRegister(INSTRUCTIONS::TargetRegister::RSI)).SetValue(5);
		static_cast<REGISTERS::GPR&>(vCore->GetRegister(INSTRUCTIONS::TargetRegister::RDI)).SetValue(6);
		static_cast<REGISTERS::GPR&>(vCore->GetRegister(INSTRUCTIONS::TargetRegister::RSP)).SetValue(7);
		static_cast<REGISTERS::GPR&>(vCore->GetRegister(INSTRUCTIONS::TargetRegister::RBP)).SetValue(8);
		static_cast<REGISTERS::GPR&>(vCore->GetRegister(INSTRUCTIONS::TargetRegister::R8)).SetValue(9);
		static_cast<REGISTERS::GPR&>(vCore->GetRegister(INSTRUCTIONS::TargetRegister::R9)).SetValue(10);
		static_cast<REGISTERS::GPR&>(vCore->GetRegister(INSTRUCTIONS::TargetRegister::R10)).SetValue(11);
		static_cast<REGISTERS::GPR&>(vCore->GetRegister(INSTRUCTIONS::TargetRegister::R11)).SetValue(12);
		static_cast<REGISTERS::GPR&>(vCore->GetRegister(INSTRUCTIONS::TargetRegister::R12)).SetValue(13);
		static_cast<REGISTERS::GPR&>(vCore->GetRegister(INSTRUCTIONS::TargetRegister::R13)).SetValue(14);
		static_cast<REGISTERS::GPR&>(vCore->GetRegister(INSTRUCTIONS::TargetRegister::R14)).SetValue(15);
		static_cast<REGISTERS::GPR&>(vCore->GetRegister(INSTRUCTIONS::TargetRegister::R15)).SetValue(16);
	}
}
