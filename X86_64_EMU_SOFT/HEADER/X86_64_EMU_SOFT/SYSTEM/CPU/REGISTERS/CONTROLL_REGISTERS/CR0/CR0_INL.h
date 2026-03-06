
#pragma once
#include "SYSTEM/CPU/EXCEPTIONS/GENERAL_PROTECTION_FAULT.h"
#include "HELPERS/GET_BIT.h"
#include "HELPERS/SET_BIT.h"
#include "CR0_DEC.h"

namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS
{
			inline void CR0::SetValue(uint64_t value)  {
				CR0::InputValidityCR0 isValid= ValidateInput(value);
				if(isValid !=InputValidityCR0::Valid)
				{
					throw  EXCEPTIONS::GENERAL_PROTECTION_FAULT("Invalid value for CR0");
				}
				storage = value;
			}



			[[nodiscard]]
			inline uint64_t CR0::GetValue() const  {
				return storage;
			}


			inline void CR0::SetPG(bool enabled) noexcept {
				storage = HELPERS::SetBit(storage, 31, enabled);
			}


			[[nodiscard]]
			inline bool CR0::GetPG() const noexcept {
				return HELPERS::GetBit(storage, 31);
			}


			inline void CR0::SetCD(bool enabled) noexcept {
				storage = HELPERS::SetBit(storage, 30, enabled);
			}


			[[nodiscard]]
			inline bool CR0::GetCD() const noexcept {
				return HELPERS::GetBit(storage, 30);
			}


			inline void CR0::SetNW(bool enabled) noexcept {
				storage = HELPERS::SetBit(storage, 29, enabled);
			}



			[[nodiscard]]
			inline bool CR0::GetNW() const noexcept {
				return HELPERS::GetBit(storage, 29);
			}


			inline void CR0::SetAM(bool enabled) noexcept {
				storage = HELPERS::SetBit(storage, 18, enabled);
			}


			[[nodiscard]]
			inline bool CR0::GetAM() const noexcept {
				return HELPERS::GetBit(storage, 18);
			}

			inline void CR0::SetWP(bool enabled) noexcept
			{
				storage = HELPERS::SetBit(storage, 16, enabled);
			}
			inline bool CR0::GetWP() const noexcept
			{
				return HELPERS::GetBit(storage, 16);
			}
			inline void CR0::SetNE(bool enabled) noexcept
			{
				storage = HELPERS::SetBit(storage, 5, enabled);
			}
			inline bool CR0::GetNE() const noexcept
			{
				return HELPERS::GetBit(storage, 5);
			}
			inline void CR0::SetET(bool enabled) noexcept
			{
				storage = HELPERS::SetBit(storage, 4, enabled);
			}
			inline bool CR0::GetET() const noexcept
			{
				return HELPERS::GetBit(storage, 4);
			}
			inline void CR0::SetTS(bool enabled) noexcept
			{
				storage = HELPERS::SetBit(storage, 3, enabled);
			}
			inline bool CR0::GetTS() const noexcept
			{
				return HELPERS::GetBit(storage, 3);
			}
			inline void CR0::SetEM(bool enabled) noexcept
			{
				storage = HELPERS::SetBit(storage, 2, enabled);
			}
			inline bool CR0::GetEM() const noexcept
			{
				return HELPERS::GetBit(storage, 2);
			}
			inline void CR0::SetMP(bool enabled) noexcept
			{
				storage = HELPERS::SetBit(storage, 1, enabled);
			}
			inline bool CR0::GetMP() const noexcept
			{
				return HELPERS::GetBit(storage, 1);
			}
			inline void CR0::SetPE(bool enabled) noexcept
			{
				storage = HELPERS::SetBit(storage, 0, enabled);
			}
			inline bool CR0::GetPE() const noexcept
			{
				return HELPERS::GetBit(storage, 0);
			}
}