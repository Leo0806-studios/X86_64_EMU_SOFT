#include <bit>
#include <cstdint>
#include <cstring>
#include <ios>
#include <memory>
#include <print>
#include <sstream>
#include <stdexcept>
#include <array>
#include <utility>
#include <format>
#include <string>
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/INSTRUCTIONS/OPCODE_BYTES.h"
#include "SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h"
#include "SYSTEM/CPU/VCORE.h"
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_ENGINE.h"
#include "SYSTEM/CPU/EXECUTION_ENGINE/EXECUTION_ENGINE.h"
#include "SYSTEM/MEMORY/MEMORY.h"
namespace X86_64_EMU_SOFT::SYSTEM::CPU {



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
			default: throw std::runtime_error("invalid value during register get");//NOSONAR
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
	uint64_t VirtualCore::GetRegisterValueMasked(INSTRUCTIONS::TargetRegister reg, uint8_t bits) const
	{
		const uint64_t value = GetRegisterValue(reg);
		const uint64_t mask = (1ULL << bits) - 1;
		const uint64_t ret = value & mask;
		return ret;
	}
	uint64_t VirtualCore::GetRegisterValueMasked(RegisterID reg, uint8_t bits) const noexcept
	{
		const uint64_t value = GetRegisterValue(reg);
		const uint64_t mask = (1ULL << bits) - 1;
		const uint64_t ret = value & mask;
		return ret;
	}
	void VirtualCore::SetRegisterValueMasked(INSTRUCTIONS::TargetRegister reg, uint64_t value, uint8_t bits)
	{
		const uint64_t originalVaue = GetRegisterValue(reg);
		const uint64_t mask = (1ULL << bits) - 1;
		const uint64_t valueToSet = value & mask;
		const uint64_t valueToKeep = originalVaue & ~mask;
		SetRegisterValue(reg, valueToKeep | valueToSet);

	}
	void VirtualCore::SetRegisterValueMasked(RegisterID reg, uint64_t value, uint8_t bits) noexcept
	{
		const uint64_t originalVaue = GetRegisterValue(reg);
		const uint64_t mask = (1ULL << bits) - 1;
		const uint64_t valueToSet = value & mask;
		const uint64_t valueToKeep = originalVaue & ~mask;
		SetRegisterValue(reg, valueToKeep | valueToSet);
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
			default: throw std::runtime_error("invalid value during register set");//NOSONAR
		}
	}
	void VirtualCore::SetRegisterValue(RegisterID reg, uint64_t value) noexcept
	{
		switch (reg) {
			case RegisterID::RAX: RAX.SetValue(value); break;
			case RegisterID::RCX: RCX.SetValue(value); break;
			case RegisterID::RDX: RDX.SetValue(value); break;
			case RegisterID::RBX: RBX.SetValue(value); break;
			case RegisterID::RSP: RSP.SetValue(value); break;
			case RegisterID::RBP: RBP.SetValue(value); break;
			case RegisterID::RSI: RSI.SetValue(value); break;
			case RegisterID::RDI: RDI.SetValue(value); break;
			default: break;
		}

	}

	[[nodiscard]]
	std::string VirtualCore::getSubregisterFromSize(CPU::INSTRUCTIONS::TargetRegister reg, uint8_t bits) {
		switch (reg) {
			using enum CPU::INSTRUCTIONS::TargetRegister;
			using enum CPU::vCoreMode;
			case RAX: switch (bits) {
				case 8: return "AL"; case 16: return "AX"; case 32:return "EAX";  case 64: default: return "RAX";
			}
			case RBX: switch (bits) {
				case 8: return "BL"; case 16: return "BX"; case 32:return "EBX";  case 64: default: return "RBX";
			}
			case RCX: switch (bits) {
				case 8: return "CL"; case 16: return "CX"; case 32:return "ECX";  case 64: default: return "RCX";
			}
			case RDX: switch (bits) {
				case 16: return "DX"; case 32:return "EDX";  case 64: default: return "RDX";
			}
			case RSI: switch (bits) {
				case 16: return "SI"; case 32:return "ESI";  case 64: default: return "RSI";
			}
			case RDI: switch (bits) {
				case 16: return "DI"; case 32:return "EDI";  case 64: default: return "RDI";
			}
			case RSP: switch (bits) {
				case 16: return "SP"; case 32:return "ESP";  case 64: default: return "RSP";
			}
			case RBP: switch (bits) {
				case 16: return "BP"; case 32:return "EBP";  case 64: default: return "RBP";
			}
			case R8:
			case R9:
			case R10:
			case R11:
			case R12:
			case R13:
			case R14:
			case R15:
			case None:
			default: return INSTRUCTIONS::RegisterToString(reg);

		}
	}

	void VirtualCore::WriteBytes(uint64_t address, const uint64_t value, uint8_t sizeBytes)
	{
		switch (sizeBytes) {
			case 1: memoryBus->Write8(address, static_cast<uint8_t>(value)); break;
			case 2: memoryBus->Write16(address, static_cast<uint16_t>(value)); break;
			case 4: memoryBus->Write32(address, static_cast<uint32_t>(value)); break;
			case 8: memoryBus->Write64(address, value); break;
			default: throw std::out_of_range("sizeBytes out of allowed range (1,2,4,8 ) for VirtualCore.WriteBytes");
		}
	}

	uint64_t VirtualCore::FetchBytes(uint64_t address, uint8_t sizeBytes) const
	{
		switch (sizeBytes) {
			case 1: return memoryBus->Read8(address); 
			case 2: return memoryBus->Read16(address); 
			case 4: return memoryBus->Read32(address); 
			case 8: return memoryBus->Read64(address); 
			default: throw std::out_of_range("sizeBytes out of allowed range (1,2,4,8 ) for VirtualCore.FetchBytes");
		}
	}

	inline void VirtualCore::PrintInstruction(const INSTRUCTIONS::Instruction& instruction) noexcept {//NOLINT(bugprone-exception-escape) Throw in std::print is non recoverable so its not handled
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

	void VirtualCore::executeInstruction(const INSTRUCTIONS::Instruction& instruction)
	{
		ExecutionEngine::ExecuteInstruction(*this, instruction);

	}
	void VirtualCore::PrintCoreState() const
	{
		std::print("*****************************************************************\n");
		std::print("RIP: {:#X}, decimal {}, signed {}\n", RIP.GetValue(), RIP.GetValue(), static_cast<int64_t>(RIP.GetValue()));
		std::print("\tEIP: {:#X}, decimal {}, signed {}\n", 0xFFFFFFFFULL & RIP.GetValue(), 0xFFFFFFFFULL & RIP.GetValue(), static_cast<int32_t>(0xFFFFFFFFULL & RIP.GetValue()));
		std::print("\tIP: {:#X}, decimal {}, signed {}\n", 0xFFFFULL & RIP.GetValue(), 0xFFFFULL & RIP.GetValue(), static_cast<int16_t>(0xFFFFULL & RIP.GetValue()));

		std::print("RAX: {:#X}, decimal {}, signed {}\n", RAX.GetValue(), RAX.GetValue(), static_cast<int64_t>(RAX.GetValue()));
		std::print("\tEAX: {:#X}, decimal {}, signed {}\n", 0xFFFFFFFFULL & RAX.GetValue(), 0xFFFFFFFFULL & RAX.GetValue(), static_cast<int32_t>(0xFFFFFFFFULL & RAX.GetValue()));
		std::print("\tAX: {:#X}, decimal {}, signed {}\n", 0xFFFFULL & RAX.GetValue(), 0xFFFFULL & RAX.GetValue(), static_cast<int16_t>(0xFFFFULL & RAX.GetValue()));

		std::print("RBX: {:#X}, decimal {}, signed {}\n", RBX.GetValue(), RBX.GetValue(), static_cast<int64_t>(RBX.GetValue()));
		std::print("\tEBX: {:#X}, decimal {}, signed {}\n", 0xFFFFFFFFULL & RBX.GetValue(), 0xFFFFFFFFULL & RBX.GetValue(), static_cast<int32_t>(0xFFFFFFFFULL & RBX.GetValue()));
		std::print("\tBX: {:#X}, decimal {}, signed {}\n", 0xFFFFULL & RBX.GetValue(), 0xFFFFULL & RBX.GetValue(), static_cast<int16_t>(0xFFFFULL & RBX.GetValue()));

		std::print("RCX: {:#X}, decimal {}, signed {}\n", RCX.GetValue(), RCX.GetValue(), static_cast<int64_t>(RCX.GetValue()));
		std::print("\tECX: {:#X}, decimal {}, signed {}\n", 0xFFFFFFFFULL & RCX.GetValue(), 0xFFFFFFFFULL & RCX.GetValue(), static_cast<int32_t>(0xFFFFFFFFULL & RCX.GetValue()));
		std::print("\tCX: {:#X}, decimal {}, signed {}\n", 0xFFFFULL & RCX.GetValue(), 0xFFFFULL & RCX.GetValue(), static_cast<int16_t>(0xFFFFULL & RCX.GetValue()));

		std::print("RDX: {:#X}, decimal {}, signed {}\n", RDX.GetValue(), RDX.GetValue(), static_cast<int64_t>(RDX.GetValue()));
		std::print("\tEDX: {:#X}, decimal {}, signed {}\n", 0xFFFFFFFFULL & RDX.GetValue(), 0xFFFFFFFFULL & RDX.GetValue(), static_cast<int32_t>(0xFFFFFFFFULL & RDX.GetValue()));
		std::print("\tDX: {:#X}, decimal {}, signed {}\n", 0xFFFFULL & RDX.GetValue(), 0xFFFFULL & RDX.GetValue(), static_cast<int16_t>(0xFFFFULL & RDX.GetValue()));

		std::print("RSI: {:#X}, decimal {}, signed {}\n", RSI.GetValue(), RSI.GetValue(), static_cast<int64_t>(RSI.GetValue()));
		std::print("\tESI: {:#X}, decimal {}, signed {}\n", 0xFFFFFFFFULL & RSI.GetValue(), 0xFFFFFFFFULL & RSI.GetValue(), static_cast<int32_t>(0xFFFFFFFFULL & RSI.GetValue()));
		std::print("\tSI: {:#X}, decimal {}, signed {}\n", 0xFFFFULL & RSI.GetValue(), 0xFFFFULL & RSI.GetValue(), static_cast<int16_t>(0xFFFFULL & RSI.GetValue()));

		std::print("RDI: {:#X}, decimal {}, signed {}\n", RDI.GetValue(), RDI.GetValue(), static_cast<int64_t>(RDI.GetValue()));
		std::print("\tEDI: {:#X}, decimal {}, signed {}\n", 0xFFFFFFFFULL & RDI.GetValue(), 0xFFFFFFFFULL & RDI.GetValue(), static_cast<int32_t>(0xFFFFFFFFULL & RDI.GetValue()));
		std::print("\tDI: {:#X}, decimal {}, signed {}\n", 0xFFFFULL & RDI.GetValue(), 0xFFFFULL & RDI.GetValue(), static_cast<int16_t>(0xFFFFULL & RDI.GetValue()));

		std::print("RSP: {:#X}, decimal {}, signed {}\n", RSP.GetValue(), RSP.GetValue(), static_cast<int64_t>(RSP.GetValue()));
		std::print("\tESP: {:#X}, decimal {}, signed {}\n", 0xFFFFFFFFULL & RSP.GetValue(), 0xFFFFFFFFULL & RSP.GetValue(), static_cast<int32_t>(0xFFFFFFFFULL & RSP.GetValue()));
		std::print("\tSP: {:#X}, decimal {}, signed {}\n", 0xFFFFULL & RSP.GetValue(), 0xFFFFULL & RSP.GetValue(), static_cast<int16_t>(0xFFFFULL & RSP.GetValue()));

		std::print("RBP: {:#X}, decimal {}, signed {}\n", RBP.GetValue(), RBP.GetValue(), static_cast<int64_t>(RBP.GetValue()));
		std::print("\tEBP: {:#X}, decimal {}, signed {}\n", 0xFFFFFFFFULL & RBP.GetValue(), 0xFFFFFFFFULL & RBP.GetValue(), static_cast<int32_t>(0xFFFFFFFFULL & RBP.GetValue()));
		std::print("\tBP: {:#X}, decimal {}, signed {}\n", 0xFFFFULL & RBP.GetValue(), 0xFFFFULL & RBP.GetValue(), static_cast<int16_t>(0xFFFFULL & RBP.GetValue()));

		std::print("R8: {:#X}, decimal {}, signed {}\n", R8.GetValue(), RBP.GetValue(), static_cast<int64_t>(R8.GetValue()));

		std::print("R9: {:#X}, decimal {}, signed {}\n", R9.GetValue(), RBP.GetValue(), static_cast<int64_t>(R9.GetValue()));


		std::print("R10: {:#X} \n", R10.GetValue());
		std::print("R11: {:#X} \n", R11.GetValue());
		std::print("R12: {:#X} \n", R12.GetValue());
		std::print("R13: {:#X} \n", R13.GetValue());
		std::print("R14: {:#X} \n", R14.GetValue());
		std::print("R15: {:#X} \n", R15.GetValue());
		std::print("CR0: {:#X}, decimal {}, binary {:#B}\n", CR0.GetValue(), CR0.GetValue(), CR0.GetValue());
		std::print("EFER: {:#X}, decimal {}, binary {:#B}\n", EFER.GetValue(), EFER.GetValue(), EFER.GetValue());

		std::print("*****************************************************************\n");

	}
	vCoreMode VirtualCore::getMode()const noexcept
	{
		vCoreMode ret = vCoreMode::realMode;
		if (EFER.GetLMA() && CR0.GetPE()) {
			ret = vCoreMode::longMode;
		}
		else if (!EFER.GetLMA() && CR0.GetPE()) {
			ret = vCoreMode::protectedMode;
		}
		return ret;

	}
	VirtualCore::VirtualCore(uint64_t resetVector, std::shared_ptr<MEMORY::MemoryBus> memBus, vCoreMode startupMode) noexcept :
		RAX(), RBX(), RCX(), RDX(), RSI(), RDI(), RSP(), RBP(), R8(), R9(), R10(), R11(), R12(), R13(), R14(), R15(),
		RIP(), EFER(), CR0(),
		isRunning(false), hasShutdown(false), isEnabled(false), memoryBus(std::move(memBus))

	{
		if (startupMode == vCoreMode::realMode) {
			CR0.SetPE(false);
			EFER.SetLME(false);
			EFER.SetLMA(false);
		}
		else if (startupMode == vCoreMode::protectedMode) {
			CR0.SetPE(true);
			EFER.SetLME(false);
			EFER.SetLMA(false);
		}
		else {
			CR0.SetPE(false);
			EFER.SetLME(true);
			EFER.SetLMA(true);
		}
		RIP.SetValue(resetVector);
	}
	VirtualCore::VirtualCore(const VirtualCore& other)noexcept :
		RAX(other.RAX), RBX(other.RBX), RCX(other.RCX), RDX(other.RDX), RSI(other.RDX), RDI(other.RDI), RSP(other.RSP), RBP(other.RBP),
		R8(other.R8), R9(other.R9), R10(other.R10), R11(other.R11), R12(other.R12), R13(other.R13), R14(other.R14), R15(other.R15),
		RIP(other.RIP), EFER(other.EFER), CR0(other.CR0),
		isRunning(other.isRunning.load()), hasShutdown(other.hasShutdown.load()), isEnabled(other.isEnabled.load()), memoryBus(other.memoryBus)
	{}
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
		RIP(std::move(other.RIP)), EFER(std::move(other.EFER)), CR0(std::move(other.CR0)),
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
	bool VirtualCore::StartCore() noexcept//NOLINT(bugprone-exception-escape) Throw in std::print is non recoverable so its not handled
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
}// namespace X86_64_EMU_SOFT::SYSTEM::CPU
