#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <unordered_map>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <print>
#include <string>
#include "SYSTEM/IO_DEVICES/MAIN_MEMORY_DEVICE.h"
#include "SYSTEM/IO_DEVICES/RESET_ROM.h"
#include "SYSTEM/SYSTEM_STATE.h"

namespace X86_64_EMU_SOFT::SYSTEM {
	namespace {


		void PrintDevices(const boost::program_options::variables_map& map) {
			if (map.count("Device") > 0) {
				std::print("Printing Devices...\n");
				for (const auto& device : map["Device"].as<std::vector<std::string>>()) {
					std::cout << "Device Descriptor: " << device << std::endl;
				}
			}
		}

	}
	bool System::ConstructAndRegisterDevices(const std::vector< std::unordered_map<DeviceDescriptorParts, std::string>>& deviceDescriptors)
	{
		for (const auto& deviceDescriptor : deviceDescriptors) {


			if (deviceDescriptor.at(DeviceDescriptorParts::DeviceType) == "MEMORY") {
				if (!RegisterMemoryDevice(deviceDescriptor)) {
					std::cerr << "failed to register memory device: ";
					for (const auto& [part, value] : deviceDescriptor) {
						std::cerr << value << " ";
					}
					std::cerr << std::endl;
					return false;
				}
			}
			else if (deviceDescriptor.at(DeviceDescriptorParts::DeviceType) == "RESET_ROM") {
				if (!RegisterResetROMDevice(deviceDescriptor)) {
					std::cerr << "failed to register reset rom device: ";
					for (const auto& [part, value] : deviceDescriptor) {
						std::cerr << value << " ";
					}
					std::cerr << std::endl;
					return false;
				}
				
			}
			else {
				std::cerr << "unknown device type: " << deviceDescriptor.at(DeviceDescriptorParts::DeviceType) << std::endl;
				return false;
			}
		}
		if (!memoryBus->BuildPageTable()) {
			std::cerr << "failed to build page table" << std::endl;
			return false;
		}
		return true;
	}


	bool System::RegisterMemoryDevice(const std::unordered_map<DeviceDescriptorParts, std::string>& deviceDescriptor)
	{
		const uint64_t sizeKB = std::stoull(deviceDescriptor.at(DeviceDescriptorParts::DeviceArg2));
		std::shared_ptr<IO_DEVICEs::DeviceBase> device = std::make_shared<IO_DEVICEs::MainMemoryDevice>(sizeKB);
		RegisteredDevices.push_back(device);
		if (!memoryBus->RegisterIODevice(device, sizeKB * 1024, 0x0)) {
			return false;
		}
		return true;
	}

	bool System::RegisterResetROMDevice(const std::unordered_map<DeviceDescriptorParts, std::string>& deviceDescriptor)
	{
		std::fstream RomFile(deviceDescriptor.at(DeviceDescriptorParts::DeviceArg2), std::ios::binary);
		if (!RomFile.is_open()) {
			std::cerr << "failed to open reset rom file: " << deviceDescriptor.at(DeviceDescriptorParts::DeviceArg2) << std::endl;
			return false;
		}
		std::vector<uint8_t> RomData((std::istreambuf_iterator<char>(RomFile)), std::istreambuf_iterator<char>());
		if (RomData.size() > 0xF) {
			std::cerr << "reset rom size exceeds 16 bytes: " << RomData.size() << " bytes" << std::endl;
			return false;
		}
		std::shared_ptr<IO_DEVICEs::DeviceBase> device = std::make_shared<IO_DEVICEs::ResetROMDevice>(RomData);
		RegisteredDevices.push_back(device);
		if (!memoryBus->RegisterIODevice(device, 0xF, this->cmdArgs.GetArgMap()["ResetVector"].as<uint32_t>())) {
			return false;
		}


		return true;
	}



	System::System(int argc, const char* argv[]) :cmdArgs(argc, argv)
	{
		if (!cmdArgs.Validate()) {
			std::cerr << "invalid command line arguments" << std::endl;
			exit(1);
		}

		PrintDevices(cmdArgs.GetArgMap());
		cpu = std::make_shared<CPU::CPU>();
		if (!cpu) {
			std::cerr << "failed to create CPU" << std::endl;
			exit(1);
		}
		std::cout << "CPU created successfully" << std::endl;
		memoryBus = std::make_shared<MEMORY::MemoryBus>();
		if (!memoryBus) {
			std::cerr << "failed to create Memory Bus" << std::endl;
			exit(1);
		}
		std::cout << "Memory Bus created successfully" << std::endl;
		std::vector< std::unordered_map<DeviceDescriptorParts, std::string>> DeviceDescriptors;
		for (const auto& descriptor : cmdArgs.GetArgMap()["Device"].as<std::vector<std::string>>()) {
			std::vector<std::string> parts;

			boost::split(parts, descriptor, boost::is_any_of(","));
			if (parts.size() != 9) {
				std::cerr << "invalid device descriptor: " << descriptor << std::endl;
				exit(1);
			}
			std::unordered_map<DeviceDescriptorParts, std::string> deviceDescriptorMap;
			for (size_t i = 0; i < parts.size() && i < 9; i++) {
				deviceDescriptorMap[static_cast<DeviceDescriptorParts>(i)] = parts[i];
			}
			DeviceDescriptors.push_back(deviceDescriptorMap);

		}
		if(!ConstructAndRegisterDevices(DeviceDescriptors)){
			std::cerr << "failed to construct and register devices" << std::endl;
			exit(1);
		}
		std::cout << "Devices constructed and registered successfully" << std::endl;
		memoryBus->PrintMemoryMap();



	}
	bool System::Start()
	{
		return false;
	}
	System::~System() noexcept
	{
	}
}