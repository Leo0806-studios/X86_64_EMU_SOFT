#include <filesystem>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>
#include <tracy/Tracy.hpp>
#include "HELPERS/CMD_PARSE.h"
#include "HELPERS/MACROS.h"
X86_64_EMU_SOFT::HELPERS::CmdArgs::CmdArgs(int argc, const char** argv)
{
	ZoneNamed(CmdArgs, true);
	namespace bo = boost::program_options;
	const char* DeviceDescriptionHelp =
		R"(this option describes IO devices in the system
the format of such descriptors is :
either
a filepath to a .deviceInfo file (the data in the file mirrors the normal syntax for the device descriptor)
or the following
DeviceName,DeviceType,HandlerBinary,DeviceArg1,DeviceArg2,DeviceArg3,DeviceArg4,DeviceArg5,DeviceArg6
the whole string must be in qoutes.
more info to the individual parts
DeviceName : simply the device wich wil be used to diferentiate devices. might be visible to the guest. must be unique
DeviceType: : describes which inbuild device type it is or custom. for more information consult the manual 
HandlerBinary : custom binary that handles device interaction insted of inbuilt versions. For requirements for the handlers pleas consult the manual 
DeviceArg1-6 : information about the device that the emulator must know. for information what those are for the types please consult the manual
)";
	boost::program_options::options_description desc("allowed options");
	desc.add_options()
		("help,h", boost::program_options::bool_switch(), "Displays Help Message")
		("StartupMode,M", bo::value<int>()->default_value(16), "sets the startup mode of the emulator,valid options are: 16(real mode), 32(protected mode) and 64(long mode)")
		("ResetVector,R", bo::value<uint64_t>()->default_value(0xFFFFFFF0), "specifies the adress at wich execution starts. has to be less than UINT32_MAX")
		("Features,X", bo::value<std::vector<std::string>>(), "specefies wich additional CPU features should be enabled(eg avx, sse,vmx,...). will set the coresponding cpuid bits and enable the registers and instructions")
		("Cores,C", bo::value<uint16_t>()->default_value(1), "sets the number of cores the emulated CPU should have. has to be between 1 and 255")
		("Device,D", bo::value<std::vector<std::string>>(), DeviceDescriptionHelp)
		("TraceMode,T",bo::value<uint16_t>()->default_value(0),"the trace logging of execution on the fly.")
		("Preset", bo::value<std::filesystem::path>(), "path to a .cpuPreset file. follows the exact same syntax as CMD arguments except that different args are seperated by newlines. do not recursively pass the same file. if a preset file is passed all other options are ignored");
	bo::variables_map ArgMap;
	bo::store(bo::parse_command_line(argc, argv, desc), ArgMap);
	if (ArgMap["help"].as<bool>()) {
		std::cout << desc << std::endl;
	}
	if (!ArgMap["Preset"].empty()) {
		std::fstream PresetFile(ArgMap["Preset"].as<std::filesystem::path>());
		if (!PresetFile.is_open()) {
			std::cerr << "failed to open preset file" << std::endl;
			exit(1);
		}
		std::vector<std::string> PresetArgs;
		std::string line;
		while (std::getline(PresetFile, line)) {
			if (!line.empty()) {
				PresetArgs.push_back(line);
			}
		}
		const char** PresetArgsCHR = new const char* [PresetArgs.size()];
		if (!PresetArgsCHR) {
			std::cerr << "failed to allocate memory for preset args" << std::endl;
			exit(1);
		}
		for (size_t i = 0; i < PresetArgs.size(); i++) {
			PresetArgsCHR[i] = PresetArgs[i].c_str();
		}


		bo::store(bo::parse_command_line(static_cast<int>(PresetArgs.size() + 1), PresetArgsCHR, desc), ArgMap);
		delete[] PresetArgsCHR;
	}
	this->argMap = std::move(ArgMap);
}

bool X86_64_EMU_SOFT::HELPERS::CmdArgs::Validate() const noexcept
{
	return true; //TODO: implement validation of command line arguments
}
