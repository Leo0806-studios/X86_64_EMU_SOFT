
#include <cstdint>
#include <bitset>
#include "HELPERS/DIFF.h"
#include "SYSTEM/CPU/REGISTERS/CONTROLL_REGISTERS/CR0.h"

namespace X86_64_EMU_SOFT
{
	namespace SYSTEM::CPU {
		namespace REGISTERS
		{
			namespace {
				/// <summary>
				/// reserved indexes are set to 1
				/// </summary>
				const std::bitset<64> ReservedEntriesMask = std::bitset<64>(0b0000000000000000000000000000000000011111111110101111111111000000);
			}
			CR0::InputValidityCR0 CR0::ValidateInput(uint64_t value) const noexcept
			{
				//check for reserved bits
				std::bitset <64> diff = HELPERS::Diff(storage, value);
				if ((diff & ReservedEntriesMask) != std::bitset<64>(0))
				{
					return InputValidityCR0::ReservedBitAccesed;
				}
				//  check if PE is set while trying to enable PG
				uint64_t peBit = HELPERS::GetBit(storage, 0);
				uint16_t pgBit = HELPERS::GetBit(value, 31);
				if (pgBit && !peBit)
				{
					return InputValidityCR0::PeInactiveOnPgEnable;
				}

				return InputValidityCR0::Valid;
			}



			

	
		}
	}
}