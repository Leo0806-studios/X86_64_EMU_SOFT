#pragma once
#include "CPU/CPU_STATE.h"
#include "SYSTEM/IO_DEVICES/DEVICE_BASE.h"
namespace X86_64_EMU_SOFT
{
	namespace SYSTEM{

		class System {
		public:
			[[nodscard]] System(int argc, const char* argv[]);

			/// <summary>
			/// begins the execution of the emulated system. This function will not return until the emulated system is stopped. The function returns true if the system was successfully started, false otherwise.
			///execution begins at the adress specified by the Reset Vector command line argument.
			/// </summary>
			/// <returns></returns>
			[[nodiscard]] bool Start();

			~System()noexcept;
		};
	}
}