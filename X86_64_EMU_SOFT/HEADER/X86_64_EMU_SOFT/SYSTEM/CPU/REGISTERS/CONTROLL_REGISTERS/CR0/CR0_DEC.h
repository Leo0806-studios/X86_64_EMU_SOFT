#pragma once
#include  <bitset>
#include <cstdint>
namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS
{
	/// <summary>
	/// reperesents CR0
	/// SetValue and GetValue are the functions used by the CPU to asignto and reat from. the individual getters and setters are for debugging and internal updates. only Get and SetValue validate integrity
	/// </summary>
	class CR0  
	{
		uint64_t storage=0;
		enum class InputValidityCR0:uint8_t {
			Valid = 0,
			ReservedBitAccesed = 1,
			PeInactiveOnPgEnable = 2
		};

		[[nodiscard]] InputValidityCR0 ValidateInput(uint64_t value) const noexcept ;
	public:
		 void SetValue(uint64_t value) ;
		[[nodiscard]]  uint64_t GetValue() const noexcept;


		//CR0 specific methods

		/// <summary>
		/// sets the PG (Paging enable) bit of CR0
		/// </summary>
		/// <param name="enabled"></param>
		void SetPG(bool enabled)noexcept;

		/// <summary>
		/// gets the value of the PG (Paging) bit of CR0
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] bool GetPG() const noexcept;

		/// <summary>
		/// sets the CD (Cache disable) bit of CR0
		/// </summary>
		/// <param name="enabled"></param>
		void SetCD(bool enabled)noexcept;

		/// <summary>
		/// gets the value of the CD (Cache disable) bit of CR0
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] bool GetCD() const noexcept;
		/// <summary>
		/// sets the NW (Not write-through) bit of CR0
		/// </summary>
		/// <param name="enabled"></param>
		void SetNW(bool enabled)noexcept;

		/// <summary>
		/// gets the value of the NW (Not write-through) bit of CR0
		/// </summary>
		/// <returns></returns>
		/// <summary>
		/// gets the value of the NW (Not write-through) bit of CR0
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] bool GetNW() const noexcept;

		/// <summary>
		/// sets the AM (Alignment mask) bit of CR0
		/// </summary>
		/// <param name="enabled"></param>
		void SetAM(bool enabled)noexcept;

		/// <summary>
		/// gets the value of the AM (Alignment mask) bit of CR0
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] bool GetAM() const noexcept;

		/// <summary>
		/// sets the WP (Write protect) bit of CR0
		/// </summary>
		/// <param name="enabled"></param>
		void SetWP(bool enabled) noexcept;

		/// <summary>
		/// gets the value of the WP (Write protect) bit of CR0
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] bool GetWP() const noexcept;

		/// <summary>
		/// sets the NE (Numeric error) bit of CR0
		/// </summary>
		/// <param name="enabled"></param>
		void SetNE(bool enabled) noexcept;

		/// <summary>
		/// gets the value of the NE (Numeric error) bit of CR0
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] bool GetNE() const noexcept;

		/// <summary>
		/// sets the ET (Extension type) bit of CR0
		/// </summary>
		/// <param name="enabled"></param>
		void SetET(bool enabled) noexcept;

		/// <summary>
		/// gets the value of the ET (Extension type) bit of CR0
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] bool GetET() const noexcept;

		/// <summary>
		/// sets the TS (Task switched) bit of CR0
		/// </summary>
		/// <param name="enabled"></param>
		void SetTS(bool enabled) noexcept;

		/// <summary>
		/// gets the value of the TS (Task switched) bit of CR0
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] bool GetTS() const noexcept;

		/// <summary>
		/// sets the EM (Emulation) bit of CR0
		/// </summary>
		/// <param name="enabled"></param>
		void SetEM(bool enabled) noexcept;
		/// <summary>
		/// gets the value of the EM (Emulation) bit of CR0
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] bool GetEM() const noexcept;

		/// <summary>
		/// sets the MP (Monitor coprocessor) bit of CR0
		/// </summary>
		/// <param name="enabled"></param>
		void SetMP(bool enabled) noexcept;
		/// <summary>
		/// gets the value of the MP (Monitor coprocessor) bit of CR0
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] bool GetMP() const noexcept;

		/// <summary>
		/// sets the PE (Protection enable) bit of CR0
		/// </summary>
		/// <param name="enabled"></param>
		void SetPE(bool enabled) noexcept;
		/// <summary>
		/// gets the value of the PE (Protection enable) bit of CR0
		/// </summary>
		/// <returns></returns>
		[[nodiscard]] bool GetPE() const noexcept;

	};
}//namespace X86_64_EMU_SOFT::SYSTEM::CPU::REGISTERS