#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include <SYSTEM/CPU/CPU_STATE.h>
#include <SYSTEM/CPU/VCORE.h>
#include <MAIN_MEMORY_DEVICE.h>
#include <SYSTEM/MEMORY/MEMORY.h>
namespace X86_64_EMU_SOFT::TESTS {
	class Boilerplate {
	public:
		/// <summary>
		/// Creates a Virtual core with zeroed out ram and a reset vector of 0.
		/// </summary>
		/// <param name="maxRamKB"></param>
		/// <param name="startupMode"></param>
		/// <returns></returns>
		[[nodiscard]] static std::unique_ptr<SYSTEM::CPU::VirtualCore> CreateVirtualCoreWithZeroRam(uint64_t maxRamKB, SYSTEM::CPU::vCoreMode startupMode);

		[[nodiscard]] static std::unique_ptr<SYSTEM::CPU::VirtualCore> CreateVirtualCore(uint64_t resetVector, std::shared_ptr<SYSTEM::MEMORY::MemoryBus> memoryBus, SYSTEM::CPU::vCoreMode startupMode);
		[[nodiscard]] static std::unique_ptr<SYSTEM::MEMORY::MemoryBus> CreateMemBus();
		[[nodiscard]] static std::shared_ptr<SYSTEM::IO_DEVICES::MainMemoryDevice> CreateMainMemoryDevice(uint64_t maxRamKB);
		[[nodiscard]] static std::shared_ptr<SYSTEM::IO_DEVICES::MainMemoryDevice> CreateMainMemoryDevicePreFilled(const std::vector<uint8_t>& bytes);
		void static SetRegistersAscending(std::shared_ptr<SYSTEM::CPU::VirtualCore>& vCore) noexcept;
	};
}