#pragma once
#include <cstdint>
#include <utility>
#include <tuple>
#include <tracy/Tracy.hpp>
#include "SYSTEM/MEMORY/MEMORY.h"
#include "SYSTEM/CPU/VCORE.h"
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_ENGINE.h"
#include <HELPERS/MACROS.h>

namespace X86_64_EMU_SOFT::SYSTEM::CPU {
	inline DEFINE_HANDLER(Handle_Prefix) {//NOSONAR
		ZoneScoped;
		std::ignore = core;
		std::ignore = address;
		switch (byte) {
			using enum X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::PrefixGroup2;
			
			using enum X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::PrefixGroup3;
			using enum X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::PrefixGroup4;
			case std::to_underlying(INSTRUCTIONS::PrefixGroup1::LOCK):
			case std::to_underlying(INSTRUCTIONS::PrefixGroup1::REPNE_REPNZ):
			case std::to_underlying(INSTRUCTIONS::PrefixGroup1::REP_REPE_REPZ): {
				instruction.Prefix1 = static_cast<INSTRUCTIONS::PrefixGroup1>(byte);
				break;
			}
			case std::to_underlying(CS_SEGMENT_OVERRIDE):
			case std::to_underlying(SS_SEGMENT_OVERRIDE):
			case std::to_underlying(DS_SEGMENT_OVERRIDE):
			case std::to_underlying(ES_SEGMENT_OVERRIDE):
			case std::to_underlying(FS_SEGMENT_OVERRIDE):
			case std::to_underlying(GS_SEGMENT_OVERRIDE): {
				instruction.Prefix2 = static_cast<INSTRUCTIONS::PrefixGroup2>(byte);
				break;
			}
			case std::to_underlying(OPERAND_SIZE_OVERRIDE): {
				prefixes.OperandSizeOverride = true;
				break;
			}
			case std::to_underlying(ADDRESS_SIZE_OVERRIDE): {
				prefixes.AddressSizeOverride = true;
				break;
			}
			default: {
				__assume(false);
			}

		}
		return false;
	}
}//namespace X86_64_EMU_SOFT::SYSTEM::CPU