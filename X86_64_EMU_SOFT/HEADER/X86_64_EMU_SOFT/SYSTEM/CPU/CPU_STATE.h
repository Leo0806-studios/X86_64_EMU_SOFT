#pragma once
#include <vector>
#include "VCORE.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU {
		class CPU {
			std::vector<VirtualCore> cores;
		};
}