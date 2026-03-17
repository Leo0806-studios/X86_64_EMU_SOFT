#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include "SYSTEM/MEMORY/MEMORY.h"
#include "VCORE.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU {
		class CPU {
			std::vector<VirtualCore> cores;
			
		public:
			CPU() = delete;
			explicit CPU(size_t numCores, uint64_t ResetVector,std::shared_ptr<MEMORY::MemoryBus> memoryBus, vCoreMode startupMode);
			bool Start();
		};
}// namespace X86_64_EMU_SOFT::SYSTEM::CPU