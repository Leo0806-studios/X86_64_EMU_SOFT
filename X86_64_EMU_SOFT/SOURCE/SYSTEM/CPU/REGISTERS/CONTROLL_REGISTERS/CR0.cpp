
#include <cstdint>
#include <bitset>
#include "HELPERS/DIFF.h"
#include "SYSTEM/CPU/REGISTERS/CONTROLL_REGISTERS/CR0.h"
#include "HELPERS/GET_BIT.h"

namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS
{
			namespace {
				/// <summary>
				/// reserved indexes are set to 1
				/// </summary>
				constexpr std::bitset<64> ReservedEntriesMask = std::bitset<64>(0b0000000000000000000000000000000000011111111110101111111111000000);
			}// namespace  
			CR0::InputValidityCR0 CR0::ValidateInput(uint64_t value) const noexcept
			{
				//check for reserved bits
				if ((std::bitset<64>(HELPERS::Diff(storage, value)) & ReservedEntriesMask) != std::bitset<64>(0))
				{
					return InputValidityCR0::ReservedBitAccesed;
				}
				//  check if PE is set while trying to enable PG
				if (HELPERS::GetBit(value, 31) && !HELPERS::GetBit(storage, 0))
				{
					return InputValidityCR0::PeInactiveOnPgEnable;
				}

				return InputValidityCR0::Valid;
			}



			

	
}// namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS