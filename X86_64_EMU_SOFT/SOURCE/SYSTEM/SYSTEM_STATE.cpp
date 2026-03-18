#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <unordered_map>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <print>
#include <cstdint>
#include <string>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <vector>
#include "SYSTEM/IO_DEVICES/MAIN_MEMORY_DEVICE.h"
#include "SYSTEM/IO_DEVICES/FIRMWARE.h"
#include "SYSTEM/IO_DEVICES/RESET_ROM.h"
#include "SYSTEM/SYSTEM_STATE.h"
#include <SYSTEM/CPU/CPU_STATE.h>
#include <SYSTEM/CPU/VCORE.h>
#include <SYSTEM/IO_DEVICES/DEVICE_BASE.h>
#include <SYSTEM/MEMORY/MEMORY.h>

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
		void LogErrorDeviceDescriptor(const std::unordered_map<System::DeviceDescriptorParts, std::string>& deviceDescriptor, const std::string& errorMessage) {
			std::cerr << errorMessage << ": ";
			for (const auto& [part, value] : deviceDescriptor) {
				std::cerr << value << " ";
			}
			std::cerr << std::endl;
		}
	}
	bool System::ConstructAndRegisterDevices(const std::vector< std::unordered_map<DeviceDescriptorParts, std::string>>& deviceDescriptors)
	{
		for (const auto& deviceDescriptor : deviceDescriptors) {


			if (deviceDescriptor.at(DeviceDescriptorParts::DeviceType) == "MEMORY") {
				if (!RegisterMemoryDevice(deviceDescriptor)) {
					LogErrorDeviceDescriptor(deviceDescriptor, "failed to register memory device");
					return false;
				}
			}
			else if (deviceDescriptor.at(DeviceDescriptorParts::DeviceType) == "RESET_ROM") {
				if (!RegisterResetROMDevice(deviceDescriptor)) {
					LogErrorDeviceDescriptor(deviceDescriptor, "failed to register reset rom device");
					return false;
				}
				
			}
			else if (deviceDescriptor.at(DeviceDescriptorParts::DeviceType) == "FIRMWARE") {
				if (!RegisterFirmwareDevice(deviceDescriptor)) {
					LogErrorDeviceDescriptor(deviceDescriptor, "failed to register firmware rom device");
					return false;
				}
			}
			else {
				std::cerr << "unknown device type: " << deviceDescriptor.at(DeviceDescriptorParts::DeviceType) << std::endl;
				return false;
			}
		}

		return true;
	}


	bool System::RegisterMemoryDevice(const std::unordered_map<DeviceDescriptorParts, std::string>& deviceDescriptor)
	{
		const uint64_t sizeKB = std::stoull(deviceDescriptor.at(DeviceDescriptorParts::DeviceArg2));
		std::shared_ptr<IO_DEVICES::DeviceBase> device = std::make_shared<IO_DEVICES::MainMemoryDevice>(sizeKB);
		RegisteredDevices.push_back(device);
		if (!memoryBus->MapMainMemory(device, sizeKB * 1024, 0x0)) {
			return false;
		}
		return true;
	}

	bool System::RegisterResetROMDevice(const std::unordered_map<DeviceDescriptorParts, std::string>& deviceDescriptor)
	{
		std::ifstream RomFile(deviceDescriptor.at(DeviceDescriptorParts::DeviceArg2), std::ios::binary);
		const auto& path = deviceDescriptor.at(DeviceDescriptorParts::DeviceArg2);
		RomFile.open(path, std::ios::binary);
		if (!RomFile.is_open()) {
			std::cerr << "failed to open reset rom file: " << deviceDescriptor.at(DeviceDescriptorParts::DeviceArg2) << std::endl;
			return false;
		}
		std::vector<uint8_t> RomData((std::istreambuf_iterator<char>(RomFile)), std::istreambuf_iterator<char>());
		if (RomData.size() > 0x10) {
			std::cerr << "reset rom size exceeds 16 bytes: " << RomData.size() << " bytes" << std::endl;
			return false;
		}
		std::shared_ptr<IO_DEVICES::DeviceBase> device = std::make_shared<IO_DEVICES::ResetROMDevice>(RomData);
		RegisteredDevices.push_back(device);
		auto reset_vector = this->cmdArgs.GetArgMap()["ResetVector"].as<uint64_t>();
		if (!memoryBus->MapResetRom(device, 0xF, reset_vector)) {
			return false;
		}


		return true;
	}

	bool System::RegisterFirmwareDevice(const std::unordered_map<DeviceDescriptorParts, std::string>& deviceDescriptor)
	{
		std::ifstream RomFile(deviceDescriptor.at(DeviceDescriptorParts::DeviceArg2), std::ios::binary);
		const auto& path = deviceDescriptor.at(DeviceDescriptorParts::DeviceArg2);
		RomFile.open(path, std::ios::binary);
		if (!RomFile.is_open()) {
			std::cerr << "failed to open Firmware rom file: " << deviceDescriptor.at(DeviceDescriptorParts::DeviceArg2) << std::endl;
			return false;
		}
		std::vector<uint8_t> RomData((std::istreambuf_iterator<char>(RomFile)), std::istreambuf_iterator<char>());

		std::shared_ptr<IO_DEVICES::DeviceBase> device = std::make_shared<IO_DEVICES::FirmwareRomDevice>(RomData);
		RegisteredDevices.push_back(device);
		auto preferedBase = std::stoull(deviceDescriptor.at(DeviceDescriptorParts::DeviceArg3));
		if (!memoryBus->MapFirmwareRom(device, RomData.size(), preferedBase)) {
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
		memoryBus = std::make_shared<MEMORY::MemoryBus>();
		if (!memoryBus) {
			std::cerr << "failed to create Memory Bus" << std::endl;
			exit(1);
		}
		std::cout << "Memory Bus created successfully" << std::endl;
		uint16_t Cores = cmdArgs.GetArgMap()["Cores"].as<uint16_t>();
		CPU::vCoreMode startupMode = CPU::IntToMode(cmdArgs.GetArgMap()["StartupMode"].as<int>());
		cpu = std::make_shared<CPU::CPU>(Cores,cmdArgs.GetArgMap()["ResetVector"].as<uint64_t>(),memoryBus,startupMode);
		if (!cpu) {
			std::cerr << "failed to create CPU" << std::endl;
			exit(1);
		}
		std::cout << "CPU created successfully" << std::endl;
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
		memoryBus->DumpMemoryToStdout();


	}
	bool System::Start()
	{
		this->cpu->Start();
		return true;
	}
	System::~System() noexcept
	{
	}
}