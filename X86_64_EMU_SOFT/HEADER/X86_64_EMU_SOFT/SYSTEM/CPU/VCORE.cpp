#include <utility>
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
				const uint8_t sibByte = memoryBus.Read8(address );
				instruction.SIB = std::bit_cast<INSTRUCTIONS::SIB>(sibByte);
				instruction.InstructionLengthBytes++;
				address++;
			}
		}
		void PrintInstruction(const INSTRUCTIONS::Instruction& instruction) {
			std::print("Instruction Length: {} bytes\n", instruction.InstructionLengthBytes);
			if (instruction.Prefix1) {
				std::print("Prefix Group 1: {:#X}\n", static_cast<uint8_t>(instruction.Prefix1));
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
			if(instruction.ImmediateSizeBytes>0){
				std::print("Immediate Bytes: ");
				for (int64_t i = instruction.ImmediateSizeBytes-1; i >=0; i--) {
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
			case 0x0F: {
				instruction.OpcodeSizeBytes = 2;
				instruction.OpcodeBytes[0] = fistOpcodeByte;
				instruction.OpcodeBytes[1] = memBus.Read8(address + 1);
				address += 2;
				instruction.InstructionLengthBytes += 2;
				break;
			}
			default: {
				instruction.OpcodeSizeBytes = 1;
				instruction.OpcodeBytes[0] = fistOpcodeByte;
				instruction.InstructionLengthBytes += 1;
				address++;
				break;
			}
		}
		if (INSTRUCTIONS::OpcodeInfos[instruction.OpcodeBytes[0]].hasMODRM) {

		DigestModRMAndSIB(address, memBus, instruction);
		instruction.hasModRM = true;
		}
		if (INSTRUCTIONS::OpcodeInfos[instruction.OpcodeBytes[0]].immediateSizeBytes) {
			instruction.ImmediateSizeBytes = INSTRUCTIONS::OpcodeInfos[instruction.OpcodeBytes[0]].immediateSizeBytes;
			switch (INSTRUCTIONS::OpcodeInfos[instruction.OpcodeBytes[0]].immediateSizeBytes) {
				case 1: {
					instruction.ImmediateBytes[0] = memBus.Read8(address);
					break;
				}
				case 2: {
					uint16_t imm16 = memBus.Read16(address);
					std::memcpy(instruction.ImmediateBytes.data(), &imm16, 2);
					break;
				}
				case 4: {
					uint32_t imm32 = memBus.Read32(address);
					std::memcpy(instruction.ImmediateBytes.data(), &imm32, 4);
					break;
				}
			}
			instruction.InstructionLengthBytes += instruction.ImmediateSizeBytes;
			address += instruction.ImmediateSizeBytes;

		}
		return instruction;
	}
	VirtualCore::VirtualCore(uint64_t resetVector, std::shared_ptr<MEMORY::MemoryBus> memoryBus) noexcept
	{
		RIP.SetValue(resetVector);
		this->memoryBus = std::move(memoryBus);
	}
	VirtualCore::VirtualCore(const VirtualCore&other)
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
	VirtualCore& VirtualCore::operator=(const VirtualCore&other)
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
		while(true){
			std::print("Decoding instruction at RIP: {:#X}\n", RIP.GetValue());
			const INSTRUCTIONS::Instruction instruction = decodeInstruction();
			RIP.SetValue(RIP.GetValue() + instruction.InstructionLengthBytes);
			PrintInstruction(instruction);
		}
		hasShutdown.store(true);
		return true;
	}

}
