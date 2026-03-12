#include <utility>
#include <string.h>
#include <bit>
#include <cstdint>
#include <memory>
#include <sstream>
#include <ios>
#include <print>
#include "SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h"
#include "INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/MEMORY/MEMORY.h"
#include "VCORE.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU {

	namespace {

		bool isPrefix[256] = { 0 };
		const bool PrefixeListSetupDone = []() {
			memset(isPrefix, 0, sizeof(isPrefix));
			isPrefix[0xF0] = true; //LOCK
			isPrefix[0xF2] = true; //REPNE/REPNZ
			isPrefix[0xF3] = true; //REP/REPE/REPZ
			isPrefix[0x2E] = true; //CS segment override
			isPrefix[0x36] = true; //SS segment override
			isPrefix[0x3E] = true; //DS segment override
			isPrefix[0x26] = true; //ES segment override
			isPrefix[0x64] = true; //FS segment override
			isPrefix[0x65] = true; //GS segment override
			isPrefix[0x66] = true; //Operand-size override
			isPrefix[0x67] = true; //Address-size override

			return true;
			}();

		void DigestPrefixes(uint64_t& address, const MEMORY::MemoryBus& memoryBus, INSTRUCTIONS::Instruction& instruction) {

			uint8_t byte = 0;
			while (byte = memoryBus.Read8(address), isPrefix[byte]) {
				switch (byte) {
					case INSTRUCTIONS::PrefixGroup1::LOCK:
					case INSTRUCTIONS::PrefixGroup1::REPNE_REPNZ:
					case INSTRUCTIONS::PrefixGroup1::REP_REPE_REPZ: {
						instruction.Prefix1 = static_cast<INSTRUCTIONS::PrefixGroup1>(byte);
						break;
					}
					case INSTRUCTIONS::PrefixGroup2::CS_SEGMENT_OVERRIDE:
					case INSTRUCTIONS::PrefixGroup2::SS_SEGMENT_OVERRIDE:
					case INSTRUCTIONS::PrefixGroup2::DS_SEGMENT_OVERRIDE:
					case INSTRUCTIONS::PrefixGroup2::ES_SEGMENT_OVERRIDE:
					case INSTRUCTIONS::PrefixGroup2::FS_SEGMENT_OVERRIDE:
					case INSTRUCTIONS::PrefixGroup2::GS_SEGMENT_OVERRIDE: {
						instruction.Prefix2 = static_cast<INSTRUCTIONS::PrefixGroup2>(byte);
						break;
					}
					case INSTRUCTIONS::PrefixGroup3::OPERAND_SIZE_OVERRIDE: {
						instruction.OperandOverride = true;
						break;
					}
					case INSTRUCTIONS::PrefixGroup4::ADDRESS_SIZE_OVERRIDE: {
						instruction.AddressOverride = true;
						break;
					}
					default: {
						__assume(false);
						break;
					}

				}
				address++;
				instruction.InstructionLengthBytes++;
			}
		}


		void DigestModRMAndSIB(uint64_t address, const MEMORY::MemoryBus& memoryBus, INSTRUCTIONS::Instruction& instruction) {
			const uint8_t modrmByte = memoryBus.Read8(address);
			instruction.ModRM = std::bit_cast<INSTRUCTIONS::ModRM>(modrmByte);
			instruction.InstructionLengthBytes++;
			address++;
			if (instruction.ModRM.mod != 3 && instruction.ModRM.rm == 4) {
				instruction.hasSIB = true;
				const uint8_t sibByte = memoryBus.Read8(address);
				instruction.SIB = std::bit_cast<INSTRUCTIONS::SIB>(sibByte);
				instruction.InstructionLengthBytes++;
				address++;
			}
		}
		void PrintInstruction(const INSTRUCTIONS::Instruction& instruction) {
			std::print("Instruction Length: {} bytes\n", instruction.InstructionLengthBytes);
			if (instruction.Prefix1) {
				std::print("Prefix Group 1: {:#X}\n", std::to_underlying(instruction.Prefix1));
			}
			if (instruction.Prefix2) {
				std::print("Prefix Group 2: {:#X}\n", static_cast<uint8_t>(instruction.Prefix2));
			}
			if (instruction.OperandOverride) {
				std::print("Operand-size override prefix is present\n");
			}
			if (instruction.AddressOverride) {
				std::print("Address-size override prefix is present\n");
			}
			std::print("Opcode Size: {} bytes\n", instruction.OpcodeSizeBytes);
			std::print("Opcode Bytes: ");
			for (size_t i = 0; i < instruction.OpcodeSizeBytes; i++) {
				std::print("{:#X} ", instruction.OpcodeBytes[i]);
			}
			std::print("\n");
			if (instruction.hasModRM) {

				std::print("ModRM: mod={}, reg={}, rm={}\n", instruction.ModRM.mod, instruction.ModRM.reg, instruction.ModRM.rm);
			}
			if (instruction.hasSIB) {
				std::print("SIB: scale={}, index={}, base={}\n", instruction.SIB.scale, instruction.SIB.index, instruction.SIB.base);
			}
			if (instruction.ImmediateSizeBytes > 0) {
				std::print("Immediate Bytes: ");
				for (int64_t i = 0; i < instruction.ImmediateSizeBytes; i++) {
					std::print("{:#X} ", instruction.ImmediateBytes[static_cast<uint64_t>(i)]);
				}
				std::print("\n");
			}

		}
	}

	INSTRUCTIONS::Instruction VirtualCore::decodeInstruction()
	{
		uint64_t address = RIP.GetValue();
		INSTRUCTIONS::Instruction instruction = {};
		const MEMORY::MemoryBus& memBus = *this->memoryBus;
		DigestPrefixes(address, memBus, instruction);
		uint8_t fistOpcodeByte = memBus.Read8(address);
		switch (fistOpcodeByte) {

			case 0xB8: //mov ax imm16
			case 0xBB: {//mov bx imm16
				instruction.OpcodeSizeBytes = 1;
				instruction.OpcodeBytes[0] = fistOpcodeByte;
				instruction.InstructionLengthBytes += 1;
				instruction.ImmediateSizeBytes = 2;
				address++;
				instruction.ImmediateBytes[0] = memBus.Read8(address);
				address++;
				instruction.ImmediateBytes[1] = memBus.Read8(address);
				instruction.InstructionLengthBytes += 2;

				break;
			}
			case 0x90: {
				//decode NOP
				instruction.OpcodeSizeBytes = 1;
				instruction.OpcodeBytes[0] = fistOpcodeByte;
				instruction.InstructionLengthBytes += 1;
				address++;
				break;
			}
			default: {
				//std::string msg = "byte: {:#X} at RIP: {:#X} coresponds to no valid opcode.", fistOpcodeByte, address;

				std::stringstream msg;
				const uint16_t asIntegerType = static_cast<uint16_t>(fistOpcodeByte);
				msg << "\n\n #UD exception \n \n byte: 0x" << std::hex << asIntegerType << " at RIP: 0x" << std::hex << address << " corresponds to no valid opcode " << std::dec;
				throw EXCEPTIONS::UNDEFINED_OPCODE(msg.str());
				break;
			}
		}

		return instruction;
	}
	void VirtualCore::executeInstruction(INSTRUCTIONS::Instruction instruction)
	{
		const uint8_t primaryOpcodeByte = instruction.OpcodeBytes[0];
		switch (primaryOpcodeByte) {

			case 0xB8: //mov ax imm16
			case 0xBB: {//mov bx imm16

				uint16_t val = 0;
				memcpy(&val, instruction.ImmediateBytes.data(), sizeof(uint16_t));
				uint8_t regSelector = primaryOpcodeByte & 0b111;
				__assume(regSelector <= 0b111);
				switch (regSelector) {
					case 0: {//ax
						const uint64_t finalVal = (RAX.GetValue() & ~(0xFFFFULL)) | val;
						RAX.SetValue(finalVal);
						break;
					}
					case 1: {//cx
						const uint64_t finalVal = (RCX.GetValue() & ~(0xFFFFULL)) | val;
						RCX.SetValue(finalVal);
						break;
					}
					case 2: {//dx
						const uint64_t finalVal = (RDX.GetValue() & ~(0xFFFFULL)) | val;
						RDX.SetValue(finalVal);
						break;
					}
					case 3: {//bx
						const uint64_t finalVal = (RBX.GetValue() & ~(0xFFFFULL)) | val;
						RBX.SetValue(finalVal);
						break;
					}
					case 4: {//sp
						const uint64_t finalVal = (RSP.GetValue() & ~(0xFFFFULL)) | val;
						RSP.SetValue(finalVal);
						break;
					}
					case 5: {//bp
						const uint64_t finalVal = (RBP.GetValue() & ~(0xFFFFULL)) | val;
						RBP.SetValue(finalVal);
						break;
					}
					case 6: {//si
						const uint64_t finalVal = (RSI.GetValue() & ~(0xFFFFULL)) | val;
						RSI.SetValue(finalVal);
						break;
					}
					case 7: {//di
						const uint64_t finalVal = (RDI.GetValue() & ~(0xFFFFULL)) | val;
						RDI.SetValue(finalVal);
						break;
					}

				}

				break;
			}
			case 0x90: {//NOP

				break;
			}

		}
	}
	void VirtualCore::PrintCoreState() const
	{
		std::print("*****************************************************************\n");
		std::print("RIP: {:#X} \n", RIP.GetValue());
		std::print("RAX: {:#X} \n", RAX.GetValue());
		std::print("RBX: {:#X} \n", RBX.GetValue());
		std::print("RCX: {:#X} \n", RCX.GetValue());
		std::print("RDX: {:#X} \n", RDX.GetValue());
		std::print("RSI: {:#X} \n", RSI.GetValue());
		std::print("RDI: {:#X} \n", RDI.GetValue());
		std::print("RSP: {:#X} \n", RSP.GetValue());
		std::print("RBP: {:#X} \n", RBP.GetValue());
		std::print("R8: {:#X} \n", R8.GetValue());
		std::print("R9: {:#X} \n", R9.GetValue());
		std::print("R10: {:#X} \n", R10.GetValue());
		std::print("R11: {:#X} \n", R11.GetValue());
		std::print("R12: {:#X} \n", R12.GetValue());
		std::print("R13: {:#X} \n", R13.GetValue());
		std::print("R14: {:#X} \n", R14.GetValue());
		std::print("R15: {:#X} \n", R15.GetValue());
		std::print("*****************************************************************\n");

	}
	VirtualCore::VirtualCore(uint64_t resetVector, std::shared_ptr<MEMORY::MemoryBus> memoryBus) noexcept
	{
		RIP.SetValue(resetVector);
		this->memoryBus = std::move(memoryBus);
	}
	VirtualCore::VirtualCore(const VirtualCore& other)
	{
		RAX = other.RAX;
		RBX = other.RBX;
		RCX = other.RCX;
		RDX = other.RDX;
		RSI = other.RSI;
		RDI = other.RDI;
		RSP = other.RSP;
		RBP = other.RBP;
		R8 = other.R8;
		R9 = other.R9;
		R10 = other.R10;
		R11 = other.R11;
		R12 = other.R12;
		R13 = other.R13;
		R14 = other.R14;
		R15 = other.R15;
		RIP = other.RIP;
		isRunning = other.isRunning.load();
		hasShutdown = other.hasShutdown.load();
		isEnabled = other.isEnabled.load();
		memoryBus = other.memoryBus;

	}
	VirtualCore& VirtualCore::operator=(const VirtualCore& other)
	{
		RAX = other.RAX;
		RBX = other.RBX;
		RCX = other.RCX;
		RDX = other.RDX;
		RSI = other.RSI;
		RDI = other.RDI;
		RSP = other.RSP;
		RBP = other.RBP;
		R8 = other.R8;
		R9 = other.R9;
		R10 = other.R10;
		R11 = other.R11;
		R12 = other.R12;
		R13 = other.R13;
		R14 = other.R14;
		R15 = other.R15;
		RIP = other.RIP;
		isRunning = other.isRunning.load();
		hasShutdown = other.hasShutdown.load();
		isEnabled = other.isEnabled.load();
		memoryBus = other.memoryBus;
		return *this;

	}
	bool VirtualCore::StartCore() noexcept
	{
		isRunning.store(true);
		hasShutdown.store(false);
		std::print("Core started at reset vector: {:#X}\n", RIP.GetValue());
		try {

			while (true) {
				std::print("\nDecoding instruction at RIP: {:#X}\n", RIP.GetValue());
				const INSTRUCTIONS::Instruction instruction = decodeInstruction();
				RIP.SetValue(RIP.GetValue() + instruction.InstructionLengthBytes);
				PrintInstruction(instruction);
				executeInstruction(instruction);
			}
		}
		catch (EXCEPTIONS::UNDEFINED_OPCODE& e) {

			std::print("{}\n", e.GetMessage());
			PrintCoreState();
		}
		hasShutdown.store(true);
		return true;
	}

}
