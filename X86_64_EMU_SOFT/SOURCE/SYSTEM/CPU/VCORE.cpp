#include <bit>
#include <cstdint>
#include <cstring>
#include <ios>
#include <memory>
#include <print>
#include <sstream>
#include <stdexcept>
#include <utility>
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/INSTRUCTIONS/OPCODE_BYTES.h"
#include "SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h"
#include "SYSTEM/CPU/VCORE.h"
#include "SYSTEM/MEMORY/MEMORY.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU { 

		class ExecutionEngine {
			
		public:
			static  void ExecuteInstruction(VirtualCore& core, const INSTRUCTIONS::Instruction& instruction);
		};
		class DecodingEngine {
		private:
			static std::array<bool,256> isPrefix;
			const static  bool PrefixListSetupDone;

			/// <summary>
			/// always digests the MODRM byte and then conditionally based on the ModRM byte it also digests the SIB byte
			/// </summary>
			/// <param name="address"></param>
			/// <param name="memoryBus"></param>
			/// <param name="instruction"></param>
			static inline void digestModRMAndSIB(uint64_t& address, const MEMORY::MemoryBus& memoryBus, INSTRUCTIONS::Instruction& instruction)noexcept;
			static inline void digestPrefixes(uint64_t& address, const MEMORY::MemoryBus& memoryBus, INSTRUCTIONS::Instruction& instruction)noexcept;

		public:
			[[nodiscard]] static  INSTRUCTIONS::Instruction DecodeInstruction(const VirtualCore& core);
		};
	namespace {










		void PrintInstruction(const INSTRUCTIONS::Instruction& instruction) {
			std::print("Instruction Length: {} bytes\n", instruction.InstructionLengthBytes);
			if (std::to_underlying(instruction.Prefix1)) {
				std::print("Prefix Group 1: {:#X}\n", std::to_underlying(instruction.Prefix1));
			}
			if (std::to_underlying(instruction.Prefix2)) {
				std::print("Prefix Group 2: {:#X}\n", std::to_underlying(instruction.Prefix2));
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
				for (uint64_t i = 0; i < instruction.ImmediateSizeBytes; i++) {
					std::print("{:#X} ", instruction.ImmediateBytes[i]);
				}
				std::print("\n");
			}

		}



		INSTRUCTIONS::TargetRegister DecodeRegisterFromModRMRegField(uint8_t regField)noexcept {
			switch (regField) {
				using enum X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::TargetRegister;
				case 0: return RAX;
				case 1: return RCX;
				case 2: return RDX;
				case 3: return RBX;
				case 4: return RSP;
				case 5: return RBP;
				case 6: return RSI;
				case 7: return RDI;
				default: {
					__assume(false);
				}
			}
		}
		INSTRUCTIONS::TargetRegister DecodeRegisterFromModRMRMField(uint8_t rmField)noexcept {
			switch (rmField) {
				using enum X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::TargetRegister;
				case 0: return RAX;
				case 1: return RCX;
				case 2: return RDX;
				case 3: return RBX;
				case 4: return RSP;
				case 5: return RBP;
				case 6: return RSI;
				case 7: return RDI;
				default: {
					__assume(false);
				}
			}
		}

		INSTRUCTIONS::TargetRegister GetTargetRegisterfromAdditiveID(uint8_t regSelector)noexcept {
			__assume(regSelector <= 0b111);
			switch (regSelector) {
				using enum X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::TargetRegister;
				case 0:return RAX;
				case 1: return RCX;
				case 2:return RDX;
				case 3:return  RBX;
				case 4:return  RSP;
				case 5:return  RBP;
				case 6:return RSI;
				case 7:return  RDI;
				default: __assume(false);

			}
		}
	}// namespace X86_64_EMU_SOFT::SYSTEM::CPU::

	INSTRUCTIONS::Instruction VirtualCore::decodeInstruction() const
	{
		return DecodingEngine::DecodeInstruction(*this);
		
	}
	uint64_t VirtualCore::GetRegisterValue(INSTRUCTIONS::TargetRegister reg) const
	{
		switch (reg) {
			case INSTRUCTIONS::TargetRegister::RAX: return RAX.GetValue();
			case INSTRUCTIONS::TargetRegister::RBX: return RBX.GetValue();
			case INSTRUCTIONS::TargetRegister::RCX: return RCX.GetValue();
			case INSTRUCTIONS::TargetRegister::RDX: return RDX.GetValue();
			case INSTRUCTIONS::TargetRegister::RSI: return RSI.GetValue();
			case INSTRUCTIONS::TargetRegister::RDI: return RDI.GetValue();
			case INSTRUCTIONS::TargetRegister::RSP: return RSP.GetValue();
			case INSTRUCTIONS::TargetRegister::RBP: return RBP.GetValue();
			case INSTRUCTIONS::TargetRegister::R8: return R8.GetValue();
			case INSTRUCTIONS::TargetRegister::R9: return R9.GetValue();
			case INSTRUCTIONS::TargetRegister::R10: return R10.GetValue();
			case INSTRUCTIONS::TargetRegister::R11: return R11.GetValue();
			case INSTRUCTIONS::TargetRegister::R12: return R12.GetValue();
			case INSTRUCTIONS::TargetRegister::R13: return R13.GetValue();
			case INSTRUCTIONS::TargetRegister::R14: return R14.GetValue();
			case INSTRUCTIONS::TargetRegister::R15: return R15.GetValue();
			case INSTRUCTIONS::TargetRegister::None:return 0xffffffffFFFFFFFFULL; 
			default: throw std::runtime_error("invalid value during register get");
		}

	}
	uint64_t VirtualCore::GetRegisterValue(RegisterID reg) const noexcept
	{
		switch (reg) {
			case RegisterID::RAX: return RAX.GetValue();
			case RegisterID::RCX: return RCX.GetValue();
			case RegisterID::RDX: return RDX.GetValue();
			case RegisterID::RBX: return RBX.GetValue();
			case RegisterID::RSP: return RSP.GetValue();
			case RegisterID::RBP: return RBP.GetValue();
			case RegisterID::RSI: return RSI.GetValue();
			case RegisterID::RDI: return RDI.GetValue();
			default: return 0xffffffffFFFFFFFFULL;
		}
		
	}
	void VirtualCore::SetRegisterValue(INSTRUCTIONS::TargetRegister reg, uint64_t value)
	{
		switch (reg) {
			case INSTRUCTIONS::TargetRegister::RAX: RAX.SetValue(value); break;
			case INSTRUCTIONS::TargetRegister::RBX: RBX.SetValue(value); break;
			case INSTRUCTIONS::TargetRegister::RCX: RCX.SetValue(value); break;
			case INSTRUCTIONS::TargetRegister::RDX: RDX.SetValue(value); break;
			case INSTRUCTIONS::TargetRegister::RSI: RSI.SetValue(value); break;
			case INSTRUCTIONS::TargetRegister::RDI: RDI.SetValue(value); break;
			case INSTRUCTIONS::TargetRegister::RSP: RSP.SetValue(value); break;
			case INSTRUCTIONS::TargetRegister::RBP: RBP.SetValue(value); break;
			case INSTRUCTIONS::TargetRegister::R8: R8.SetValue(value); break;
			case INSTRUCTIONS::TargetRegister::R9: R9.SetValue(value); break;
			case INSTRUCTIONS::TargetRegister::R10: R10.SetValue(value); break;
			case INSTRUCTIONS::TargetRegister::R11: R11.SetValue(value); break;
			case INSTRUCTIONS::TargetRegister::R12: R12.SetValue(value); break;
			case INSTRUCTIONS::TargetRegister::R13: R13.SetValue(value); break;
			case INSTRUCTIONS::TargetRegister::R14: R14.SetValue(value); break;
			case INSTRUCTIONS::TargetRegister::R15: R15.SetValue(value); break;
			case INSTRUCTIONS::TargetRegister::None: break;
			default: throw std::runtime_error("invalid value during register set");
		}
	}
	void VirtualCore::SetRegisterValue(RegisterID reg, uint64_t value) noexcept
	{
		switch (reg) {
			case RegisterID::RAX: RAX.SetValue(value);break;
			case RegisterID::RCX: RCX.SetValue(value);break;
			case RegisterID::RDX: RDX.SetValue(value);break;
			case RegisterID::RBX: RBX.SetValue(value);break;
			case RegisterID::RSP: RSP.SetValue(value);break;
			case RegisterID::RBP: RBP.SetValue(value);break;
			case RegisterID::RSI: RSI.SetValue(value);break;
			case RegisterID::RDI: RDI.SetValue(value); break;
			default: break;
	}
	
	}
	
	
	void VirtualCore::executeInstruction(const INSTRUCTIONS::Instruction& instruction)
	{
		ExecutionEngine::ExecuteInstruction(*this, instruction);

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
	VirtualCore::VirtualCore(uint64_t resetVector, std::shared_ptr<MEMORY::MemoryBus> memoryBus) noexcept :
		RAX(), RBX(), RCX(), RDX(), RSI(), RDI(), RSP(), RBP(), R8(), R9(), R10(), R11(), R12(), R13(), R14(), R15(),
		RIP(),
		isRunning(false), hasShutdown(false), isEnabled(false), memoryBus(std::move(memoryBus))

	{
		RIP.SetValue(resetVector);
	}
	VirtualCore::VirtualCore(const VirtualCore& other)noexcept :
		RAX(other.RAX), RBX(other.RBX), RCX(other.RCX), RDX(other.RDX), RSI(other.RDX), RDI(other.RDI), RSP(other.RSP), RBP(other.RBP),
		R8(other.R8), R9(other.R9), R10(other.R10), R11(other.R11), R12(other.R12), R13(other.R13), R14(other.R14), R15(other.R15),
		RIP(other.RIP),
		isRunning(other.isRunning.load()), hasShutdown(other.hasShutdown.load()), isEnabled(other.isEnabled.load()), memoryBus(other.memoryBus)
	{
	}
	VirtualCore& VirtualCore::operator=(const VirtualCore& other)noexcept
	{
		if (this == &other) {
			return *this;
		}
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
	VirtualCore::VirtualCore(VirtualCore&& other)noexcept :
		RAX(std::move(other.RAX)), RBX(std::move(other.RBX)), RCX(std::move(other.RCX)), RDX(std::move(other.RDX)), RSI(std::move(other.RSI)), RDI(std::move(other.RDI)),
		RSP(std::move(other.RSP)), RBP(std::move(other.RBP)), R8(std::move(other.R8)), R9(std::move(other.R9)), R10(std::move(other.R10)), R11(std::move(other.R11)),
		R12(std::move(other.R12)), R13(std::move(other.R13)), R14(std::move(other.R14)), R15(std::move(other.R15)),
		RIP(std::move(other.RIP)),
		isRunning(other.isRunning.load()), hasShutdown(other.hasShutdown.load()), isEnabled(other.isEnabled.load()), memoryBus(std::move(other.memoryBus))
	{}
	VirtualCore& VirtualCore::operator=(VirtualCore&& other) noexcept
	{
		if (this == &other) {
			return *this;
		}
		RAX = std::move(other.RAX);
		RBX = std::move(other.RBX);
		RCX = std::move(other.RCX);
		RDX = std::move(other.RDX);
		RSI = std::move(other.RSI);
		RDI = std::move(other.RDI);
		RSP = std::move(other.RSP);
		RBP = std::move(other.RBP);
		R8 = std::move(other.R8);
		R9 = std::move(other.R9);
		R10 = std::move(other.R10);
		R11 = std::move(other.R11);
		R12 = std::move(other.R12);
		R13 = std::move(other.R13);
		R14 = std::move(other.R14);
		R15 = std::move(other.R15);
		RIP = std::move(other.RIP);
		isRunning = other.isRunning.load();
		hasShutdown = other.hasShutdown.load();
		isEnabled = other.isEnabled.load();
		memoryBus = std::move(other.memoryBus);
		return *this;
	}
	VirtualCore::~VirtualCore()
	{
		isRunning.store(false);
		hasShutdown.store(true);

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














	std::array<bool, 256> DecodingEngine::isPrefix = { };
	const bool DecodingEngine::PrefixListSetupDone = []() noexcept{
		memset(&isPrefix[0], 0, sizeof(isPrefix));
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




	inline void DecodingEngine::digestModRMAndSIB(uint64_t& address, const MEMORY::MemoryBus& memoryBus, INSTRUCTIONS::Instruction& instruction) noexcept
	{
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

	inline void DecodingEngine::digestPrefixes(uint64_t& address, const MEMORY::MemoryBus& memoryBus, INSTRUCTIONS::Instruction& instruction) noexcept
	{
		uint8_t byte = 0;
		while (byte = memoryBus.Read8(address), isPrefix[byte]) {//NOSONAR 
			switch (byte) {
				using enum X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::PrefixGroup2;
				using enum X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::PrefixGroup1;
				using enum X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::PrefixGroup3;
				using enum X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::PrefixGroup4;
				case std::to_underlying(LOCK):
				case std::to_underlying(REPNE_REPNZ):
				case std::to_underlying(REP_REPE_REPZ): {
					instruction.Prefix1 = static_cast<INSTRUCTIONS::PrefixGroup1>(byte);
					break;
				}
				case std::to_underlying(CS_SEGMENT_OVERRIDE):
				case std::to_underlying(SS_SEGMENT_OVERRIDE):
				case std::to_underlying(DS_SEGMENT_OVERRIDE):
				case std::to_underlying(ES_SEGMENT_OVERRIDE):
				case std::to_underlying(FS_SEGMENT_OVERRIDE):
				case std::to_underlying(GS_SEGMENT_OVERRIDE): {
					instruction.Prefix2 = static_cast<INSTRUCTIONS::PrefixGroup2>(byte);
					break;
				}
				case std::to_underlying(OPERAND_SIZE_OVERRIDE): {
					instruction.OperandOverride = true;
					break;
				}
				case std::to_underlying(ADDRESS_SIZE_OVERRIDE): {
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

	inline INSTRUCTIONS::Instruction DecodingEngine::DecodeInstruction(const VirtualCore& core)
	{


		uint64_t address = core.RIP.GetValue();
		INSTRUCTIONS::Instruction instruction = {};
		const MEMORY::MemoryBus& memBus = *core.memoryBus;
		digestPrefixes(address, memBus, instruction);
		const uint8_t fistOpcodeByte = memBus.Read8(address);
		address++;
		instruction.OpcodeBytes[0] = fistOpcodeByte;
		instruction.OpcodeSizeBytes = 1;
		instruction.InstructionLengthBytes++;
		switch (fistOpcodeByte) {
			using enum INSTRUCTIONS::PrimaryOpcodeByteValue;
			case std::to_underlying(ADDrm16r16): {//ADD r/m16, r16
				instruction.SourceSize = 16;
				instruction.DestinationSize = 16;
				instruction.hasModRM = true;
				instruction.Type = INSTRUCTIONS::InstructionType::ADD;
				digestModRMAndSIB(address,memBus,instruction);
				instruction.SourceRegister = DecodeRegisterFromModRMRegField(instruction.ModRM.reg);
				if (instruction.ModRM.mod == 3) {
					instruction.DestinationRegister = DecodeRegisterFromModRMRMField(instruction.ModRM.rm);
				}
				break;
			}
			case std::to_underlying(MOVr16r32imm16imm32_BASE) + 0: //mov ax imm16
			case std::to_underlying(MOVr16r32imm16imm32_BASE) + 1:
			case std::to_underlying(MOVr16r32imm16imm32_BASE) + 2:
			case std::to_underlying(MOVr16r32imm16imm32_BASE) + 3:
			case std::to_underlying(MOVr16r32imm16imm32_BASE) + 4:
			case std::to_underlying(MOVr16r32imm16imm32_BASE) + 5:
			case std::to_underlying(MOVr16r32imm16imm32_BASE) + 6:
			case std::to_underlying(MOVr16r32imm16imm32_BASE) + 7: {
				if (instruction.OperandOverride) {
					instruction.SourceSize = 32;
					instruction.DestinationSize = 32;
				}
				else {
					instruction.SourceSize = 16;
					instruction.DestinationSize = 16;
				}
				instruction.Type = INSTRUCTIONS::InstructionType::MOV;
				instruction.ImmediateSizeBytes = 2;
				instruction.ImmediateBytes[0] = memBus.Read8(address);
				instruction.DestinationRegister = GetTargetRegisterfromAdditiveID(static_cast<uint8_t>(instruction.OpcodeBytes[0] & static_cast<uint8_t>(0b111)));
				address++;
				instruction.ImmediateBytes[1] = memBus.Read8(address);
				instruction.InstructionLengthBytes += 2;

				break;
			}
			case std::to_underlying(NOP): {
				instruction.Type = INSTRUCTIONS::InstructionType::NOP;
				//decode NOP

				break;
			}
			case std::to_underlying(UD): {
				instruction.Type = INSTRUCTIONS::InstructionType::UD;
				break;
			}
			default: {

				std::stringstream msg;
				const auto asIntegerType = static_cast<uint16_t>(fistOpcodeByte);
				msg << "\n\n #UD exception \n \n byte: 0x" << std::hex << asIntegerType << " at RIP: 0x" << std::hex << address << " corresponds to no valid opcode " << std::dec;
				throw EXCEPTIONS::UNDEFINED_OPCODE(msg.str());
				break;
			}
		}

		return instruction;
	}

	inline void ExecutionEngine::ExecuteInstruction(VirtualCore& core, const INSTRUCTIONS::Instruction& instruction)
	{
		switch (instruction.Type) {
			using enum INSTRUCTIONS::InstructionType;
			case MOV: {
				if (instruction.ImmediateSizeBytes > 0) {
					std::print("Executing instruction: ADD r/m{}, imm{}\n", instruction.DestinationSize, instruction.SourceSize);
				uint64_t val = 0;
				__assume(instruction.ImmediateBytes.data());
				memcpy(&val, instruction.ImmediateBytes.data(), sizeof(uint64_t));
				uint64_t tmpval =core.GetRegisterValue(instruction.DestinationRegister);
				const uint64_t mask = (1ULL << instruction.DestinationSize) - 1;
				tmpval &= ~mask;
				tmpval |= val;
				std::print("value of destination register before\n");
				core.SetRegisterValue(instruction.DestinationRegister, tmpval);
				break;
				}
				break;
			}
			case ADD: {
				if (instruction.ImmediateSizeBytes > 0) {
					throw EXCEPTIONS::UNDEFINED_OPCODE("immediat operands are not supported yet for ADD");
				}


					if (instruction.ModRM.mod != 0b11) {
					throw EXCEPTIONS::UNDEFINED_OPCODE("Memory operands are not supported yet for ADD");
					}

				std::print("Executing instruction: ADD r/m{}, r{}\n",instruction.DestinationSize,instruction.SourceSize);
				const uint64_t sourceVal = core.GetRegisterValue(instruction.SourceRegister);
				const uint64_t destVal = core.GetRegisterValue(instruction.DestinationRegister);
				std::print("value of Destintion register: {:#X}, value of Source Register {:#X} before execution\n", destVal, sourceVal);
				core.SetRegisterValue(instruction.DestinationRegister, destVal + sourceVal);
				std::print("value of Destiontion Register: {:#X} after execution\n",core.GetRegisterValue(instruction.DestinationRegister));
				


				break;

			}
			case UD: {
				throw EXCEPTIONS::UNDEFINED_OPCODE(std::format("UD instruction executed at RIP: {:#X}", core.RIP.GetValue()));
			}
			case NOP: {
				std::print("Executing instruction NOP\n");
				break;
			}
		}
	}

}
