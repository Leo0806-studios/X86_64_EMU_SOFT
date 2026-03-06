#include <boost/program_options.hpp>
#include <filesystem>
#include <limits>
#include "SYSTEM/SYSTEM_STATE.h"

namespace X86_64_EMU_SOFT::SYSTEM {
	namespace {
		/// <summary>
		/// parses the command line and returns the variable map
		/// </summary>
		/// <param name="argc"></param>
		/// <param name="argv"></param>
		/// <returns></returns>
		boost::program_options::variables_map ParseCMD(int argc, const char** argv) {
			 namespace bo =boost::program_options;
			 const char* DeviceDescriptionHelp =
				 R"(this option describes IO devices in the system
the format of such descriptors is :
either
a filepath to a .deviceInfo file (the data in the file mirrors the normal syntax for the device descriptor)
or the following
DeviceName,DeviceType,HandlerBinary,DeviceArg1,DeviceArg2,DeviceArg3,DeviceArg4,DeviceArg5,DeviceArg6
the whole string must be in qoutes.
more info to the individual parts
DeviceName : simply the device wich wil be used to diferentiate defices. might be visible to the guest. must be unique
DeviceType: : describes which inbuild device type it is or custom. allowed values: RAM, GPU, USB, STORAGE, AUDIO, CRYPT, NETWORK, CUSTOM. for more information consult the manual (1.5.2 DeviceTypes)
HandlerBinary : custom binary that handles device interaction insted of inbuild versions. For requirements for the handlers pleas consult the manual (2.4.1 Custom Handlers)
DeviceArg1-6 : information about the device that the emulator must know. for information what those are for the types please consult the manual (2.4.0 Device Identity)
)";
			boost::program_options::options_description desc("alowed options");
			desc.add_options()
				("StartupMode,M", bo::value<int>()->default_value(16), "sets the startup mode of the emulator,valid options are: 16(real mode), 32(protected mode) and 64(long mode)")
				("ResetVector,R", bo::value<uint64_t>()->default_value(0xFFFFFFF0), "specefies the adress at wich execution starts. has to be less than UINT32_MAX")
				("Firmware,F", bo::value<std::filesystem::path>(), "the firmware binary. has to be a flat binary")
				("ResetBytes,B", bo::value < std::filesystem::path>(), "path to a small flat binary tha gets copied to the reset vector (r) to r+15 . only the first 16 bytes of the binary are copied")
				("Features,X", bo::value<std::vector<std::string>>(), "specefies wich additional CPU features should be enabled(eg avx, sse,vmx,...). will set the coresponding cpuid bits and enable the registers and instructions")
				("Device,D", bo::value<std::vector<std::string>>(), DeviceDescriptionHelp)
				("Preset",bo::value<std::filesystem::path>(),"path to a .cpuPreset file. follows the exact same syntax as CMD arguments. do not recursively pass the same file");
			bo::variables_map ArgMap;
			bo::store(bo::parse_command_line(argc, argv, desc), ArgMap);
			return ArgMap;
		}
		[[nodiscard]] bool ValidateCMD(const boost::program_options::variables_map map) {

		}

	}	
	System::System(int argc, const char* argv[])
	{
		boost::program_options::variables_map cmdArgs = ParseCMD(argc,argv)

	}
}