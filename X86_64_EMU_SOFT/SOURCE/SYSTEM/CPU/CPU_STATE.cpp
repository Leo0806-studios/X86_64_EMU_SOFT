#include <thread>
#include <print>
#include "SYSTEM/CPU/CPU_STATE.h"

namespace X86_64_EMU_SOFT::SYSTEM::CPU{
	CPU::CPU(size_t numCores, uint64_t ResetVector,std::shared_ptr<MEMORY::MemoryBus>memoryBus,vCoreMode startupMode)
	{
		for (uint64_t i = 0; i < numCores; i++) {
			VirtualCore core(ResetVector, memoryBus,startupMode);
			cores.push_back(std::move(core));
		}
	}
	bool CPU::Start() 
	{
		std::print("Starting CPU with {} cores\n", cores.size());
		std::print("selectet BSP: 0\n");
		std::thread bspThread(&VirtualCore::StartCore, &cores[0]);
		while (!cores[0].hasShutdownCore()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		bspThread.join();
		return true;
	}
}