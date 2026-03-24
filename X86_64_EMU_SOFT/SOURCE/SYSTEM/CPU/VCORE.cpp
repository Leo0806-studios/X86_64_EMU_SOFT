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
#include <source_location>
#include <tracy/Tracy.hpp>
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
#include "SYSTEM/CPU/INSTRUCTIONS/OPCODE_BYTES.h"
#include "SYSTEM/CPU/EXCEPTIONS/UNDEFINED_OPCODE.h"
#include "SYSTEM/CPU/VCORE.h"
#include "SYSTEM/CPU/DECODING_ENGINE/DECODING_ENGINE.h"
#include "SYSTEM/CPU/EXECUTION_ENGINE/EXECUTION_ENGINE.h"
#include "SYSTEM/MEMORY/MEMORY.h"
#include "HELPERS/GLOBALS.h"
#include <HELPERS/MACROS.h>

namespace X86_64_EMU_SOFT::SYSTEM::CPU {



	INSTRUCTIONS::Instruction VirtualCore::decodeInstruction() const
	{
		ZoneScoped;//NOLINT
		uint64_t address = RIP.GetValue();
		INSTRUCTIONS::Instruction instruction = {};

		auto instructionByte = static_cast<uint8_t>(FetchBytes(address, 1));
		address++;
		INSTRUCTIONS::Prefixes prefixes = {};
		while (!DecodingEngine::HandlerFuncs[instructionByte](*this, address, instruction,prefixes, instructionByte)) {
			instructionByte = static_cast<uint8_t>(FetchBytes(address, 1));
			address++;
		}
		return instruction;


	}








	[[nodiscard]]
	std::string VirtualCore::getSubregisterFromSize(CPU::INSTRUCTIONS::TargetRegister reg, uint8_t bits) {
		DeepZoneScoped;//NOLINT
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
				case 8:return "DL";	case 16: return "DX"; case 32:return "EDX";  case 64: default: return "RDX";
			}
			case RSI: switch (bits) {
				case 8: return "SIL"; case 16: return "SI"; case 32:return "ESI";  case 64: default: return "RSI";
			}
			case RDI: switch (bits) {
				case 8: return "DIL"; case 16: return "DI"; case 32:return "EDI";  case 64: default: return "RDI";
			}
			case RSP: switch (bits) {
				case 8: return "SPL"; case 16: return "SP"; case 32:return "ESP";  case 64: default: return "RSP";
			}
			case RBP: switch (bits) {
				case 8: return "BPL"; case 16: return "BP"; case 32:return "EBP";  case 64: default: return "RBP";
			}
			case R8: switch (bits)
			{
				case 8: return "R8B"; case 16: return "R8W"; case 32:return "R8D";  case 64: default: return "R8";
			}
			case R9: switch (bits)
			{
				case 8: return "R9B"; case 16: return "R9W"; case 32:return "R9D";  case 64: default: return "R9";
			}
			case R10: switch (bits)
			{
				case 8: return "R10B"; case 16: return "R10W"; case 32:return "R10D";  case 64: default: return "R10";
			}
			case R11: switch (bits)
			{
				case 8: return "R11B"; case 16: return "R11W"; case 32:return "R11D";  case 64: default: return "R11";
			}
			case R12: switch (bits) {
				case 8: return "R12B"; case 16: return "R12W"; case 32:return "R12D";  case 64: default: return "R12";
			}
			case R13: switch (bits) {
				case 8: return "R13B"; case 16: return "R13W"; case 32:return "R13D";  case 64: default: return "R13";
			}
			case R14: switch (bits) {
				case 8: return "R14B"; case 16: return "R14W"; case 32:return "R14D";  case 64: default: return "R14";
			}
			case R15: switch (bits) {
				case 8: return "R15B"; case 16: return "R15W"; case 32:return "R15D";  case 64: default: return "R15";
			}
			case CR0: return "CR0";
			case MSR_EFER: return "EFER";
			case AH: return "AH";
			case BH: return "BH";
			case CH: return "CH";
			case DH: return "DH";
			case None: return "None";
			default: return "Undefined";

		}
	}

	void VirtualCore::WriteBytes(uint64_t address, const uint64_t value, uint8_t sizeBytes)
	{
		ZoneScoped;//NOLINT
		switch (sizeBytes) {
			case 1: memoryBus->Write8(address, static_cast<uint8_t>(value)); break;
			case 2: memoryBus->Write16(address, static_cast<uint16_t>(value)); break;
			case 4: memoryBus->Write32(address, static_cast<uint32_t>(value)); break;
			case 8: memoryBus->Write64(address, value); break;
			default: throw std::out_of_range("sizeBytes out of allowed range (1,2,4,8 ) for VirtualCore.WriteBytes");
		}
	}

	inline	uint64_t  VirtualCore::FetchBytes(uint64_t address, uint8_t sizeBytes) const
	{
		ZoneScoped;//NOLINT
		switch (sizeBytes) {
			[[likely]] case 1: return memoryBus->Read8(address);
			case 2: return memoryBus->Read16(address);
			case 4: return memoryBus->Read32(address);
			case 8: return memoryBus->Read64(address);
			default: throw std::out_of_range("sizeBytes out of allowed range (1,2,4,8 ) for VirtualCore.FetchBytes");
		}
	}

	inline void VirtualCore::PrintInstruction(const INSTRUCTIONS::Instruction& instruction) noexcept {//NOLINT(bugprone-exception-escape) Throw in std::print is non recoverable so its not handled
		ZoneScoped;//NOLINT
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
		ZoneScoped;//NOLINT
		ExecutionEngine::HandlerFuncs[std::to_underlying(instruction.Type)](*this, instruction);


	}
	void VirtualCore::PrintCoreState() const
	{
		ZoneScoped;//NOLINT
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
	VirtualCore::VirtualCore(uint64_t resetVector, std::shared_ptr<MEMORY::MemoryBus> memBus, vCoreMode startupMode) noexcept :
		RAX(), RBX(), RCX(), RDX(), RSI(), RDI(), RSP(), RBP(), R8(), R9(), R10(), R11(), R12(), R13(), R14(), R15(),
		RIP(), EFER(), CR0(),
		isRunning(false), hasShutdown(false), isEnabled(false), memoryBus(std::move(memBus))

	{
		DeepZoneScoped;
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
	{
		DeepZoneScoped;
	}
	VirtualCore& VirtualCore::operator=(const VirtualCore& other)noexcept
	{
		DeepZoneScoped;
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
	{
		DeepZoneScoped;
	}
	VirtualCore& VirtualCore::operator=(VirtualCore&& other) noexcept
	{
		DeepZoneScoped;
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
		DeepZoneScoped;
		isRunning.store(false);
		hasShutdown.store(true);

	}
	bool VirtualCore::StartCore() noexcept//NOLINT(bugprone-exception-escape) Throw in std::print is non recoverable so its not handled
	{

		DeepZoneScoped;
		isRunning.store(true);
		hasShutdown.store(false);
		std::print("Core started at reset vector: {:#X}\n", RIP.GetValue());
		try {

			while (true) {
				FrameMark;
				RunIfMinimalOrHigherTraceMode(std::print("\nDecoding instruction at RIP: {:#X}\n", RIP.GetValue());)

					const INSTRUCTIONS::Instruction instruction = decodeInstruction();
				RIP.SetValue(RIP.GetValue() + instruction.InstructionLengthBytes);
				RunIfReducedOrHigherTraceMode(PrintInstruction(instruction);)
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

	REGISTERS::Register& VirtualCore::GetRegister(INSTRUCTIONS::TargetRegister reg) noexcept
	{
		switch (reg) {
			case INSTRUCTIONS::TargetRegister::RAX: return RAX;
			case INSTRUCTIONS::TargetRegister::RBX: return RBX;
			case INSTRUCTIONS::TargetRegister::RCX: return RCX;
			case INSTRUCTIONS::TargetRegister::RDX: return RDX;
			case INSTRUCTIONS::TargetRegister::RSI: return RSI;
			case INSTRUCTIONS::TargetRegister::RDI: return RDI;
			case INSTRUCTIONS::TargetRegister::RSP: return RSP;
			case INSTRUCTIONS::TargetRegister::RBP: return RBP;
			case INSTRUCTIONS::TargetRegister::R8: return R8;
			case INSTRUCTIONS::TargetRegister::R9: return R9;
			case INSTRUCTIONS::TargetRegister::R10: return R10;
			case INSTRUCTIONS::TargetRegister::R11: return R11;
			case INSTRUCTIONS::TargetRegister::R12: return R12;
			case INSTRUCTIONS::TargetRegister::R13: return R13;
			case INSTRUCTIONS::TargetRegister::R14: return R14;
			case INSTRUCTIONS::TargetRegister::R15: return R15;
			case INSTRUCTIONS::TargetRegister::CR0: return CR0;
			case INSTRUCTIONS::TargetRegister::MSR_EFER: return EFER;
			case INSTRUCTIONS::TargetRegister::AH: return RAX;
			case INSTRUCTIONS::TargetRegister::BH: return RBX;
			case INSTRUCTIONS::TargetRegister::CH: return RCX;
			case INSTRUCTIONS::TargetRegister::DH: return RDX;
			case INSTRUCTIONS::TargetRegister::None:
			default: NeverOrAssert(false);
		}
		__assume(false);
	}

}// namespace X86_64_EMU_SOFT::SYSTEM::CPU
