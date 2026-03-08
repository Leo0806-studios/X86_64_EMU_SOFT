#pragma once
#include <vector>

#include <unordered_map>
#include <memory>
#include "CPU/CPU_STATE.h"
#include "SYSTEM/IO_DEVICES/DEVICE_BASE.h"
#include "SYSTEM/MEMORY/MEMORY.h"
#include "HELPERS/CMD_PARSE.h"

namespace X86_64_EMU_SOFT
{
	namespace SYSTEM{

		class System {
			std::vector<std::shared_ptr<IO_DEVICEs::DeviceBase>> RegisteredDevices;
			std::shared_ptr<CPU::CPU> cpu;
			std::shared_ptr<MEMORY::MemoryBus> memoryBus;
			const HELPERS::CmdArgs cmdArgs;
			enum class DeviceDescriptorParts
			{
				DeviceName = 0,
				DeviceType = 1,
				HandlerBinary = 2,
				DeviceArg1 = 3,
				DeviceArg2 = 4,
				DeviceArg3 = 5,
				DeviceArg4 = 6,
				DeviceArg5 = 7,
				DeviceArg6 = 8
			};
			[[nodiscard]] bool ConstructAndRegisterDevices(const std::vector< std::unordered_map<DeviceDescriptorParts, std::string>>& deviceDescriptors);
			[[nodiscard]] bool RegisterMemoryDevice(const std::unordered_map<DeviceDescriptorParts, std::string>& deviceDescriptor);
			[[nodiscard]] bool RegisterResetROMDevice(const std::unordered_map<DeviceDescriptorParts, std::string>& deviceDescriptor);
			[[nodiscard]] bool RegisterFirmwareDevice(const std::unordered_map<DeviceDescriptorParts, std::string>& deviceDescriptor);
		public:
			[[nodiscard]] System(int argc, const char* argv[]);

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