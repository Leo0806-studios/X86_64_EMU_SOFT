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
#include <SYSTEM/CPU/DECODING_ENGINE/DECODING_HANDLERS/HANDLERS_ALU.h>
#include <SYSTEM/CPU/DECODING_ENGINE/DECODING_HANDLERS/HANDLERS_SPECIAL.h>
#include <SYSTEM/CPU/DECODING_ENGINE/DECODING_HANDLERS/PREFIX_HANDLERS.h>
#include "SYSTEM/MEMORY/MEMORY.h"
#include "HELPERS/GLOBALS.h"
#include <HELPERS/MACROS.h>
#include <cstdarg>

namespace X86_64_EMU_SOFT::SYSTEM::CPU {
	[[noreturn]] inline static bool ThrowUndefinedOpcode(VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, INSTRUCTIONS::Prefixes& prefixes, uint8_t byte) {//NOSONAR
		DeepZoneScoped;//NOLINT
		std::ignore = core;
		std::ignore = instruction;
		std::ignore = prefixes;
		std::stringstream msg;
		const auto asIntegerType = static_cast<uint16_t>(byte);
		msg << "\n\n #UD exception \n \n byte: 0x" << std::hex << asIntegerType << " at RIP: 0x" << std::hex << address << " corresponds to no valid opcode " << std::dec;
		throw EXCEPTIONS::UNDEFINED_OPCODE(msg.str());
	}
	using HandlerFunc = bool(*)(VirtualCore& core, uint64_t& address, INSTRUCTIONS::Instruction& instruction, INSTRUCTIONS::Prefixes& prefixes, uint8_t byte);
	constexpr  std::array<HandlerFunc, 256> HandlerFuncs =  []() consteval noexcept {
		std::array<HandlerFunc, 256> arr{};
		for (auto& func : arr) {
			func = ThrowUndefinedOpcode;
		}
		arr[0xF0] = Handle_Prefix; //LOCK
		arr[0xF2] = Handle_Prefix; //REPNE/REPNZ
		arr[0xF3] = Handle_Prefix; //REP/REPE/REPZ
		arr[0x2E] = Handle_Prefix; //CS segment override
		arr[0x36] = Handle_Prefix; //SS segment override
		arr[0x3E] = Handle_Prefix; //DS segment override
		arr[0x26] = Handle_Prefix; //ES segment override
		arr[0x64] = Handle_Prefix; //FS segment override
		arr[0x65] = Handle_Prefix; //GS segment override
		arr[0x66] = Handle_Prefix; //Operand-size override
		arr[0x67] = Handle_Prefix; //Address-size override

		//ALU
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::ADD_rm8_r8_0x0)] = Handler_rm8_r8<INSTRUCTIONS::InstructionType::ADD>;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::ADD_rm16rm32rm64_r16r32r64_0x1)] = Handle_r16r32r64_rm16rm32rm64<INSTRUCTIONS::InstructionType::ADD>;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::ADD_r8_rm8_0x2)] = Handle_r8_rm8<INSTRUCTIONS::InstructionType::ADD>;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::ADD_r16r32r64_rm16rm32rm64_0x3)] = Handle_r16r32r64_rm16rm32rm64<INSTRUCTIONS::InstructionType::ADD>;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::ADD_AL_imm8_0x4)] = Handle_AL_imm8<INSTRUCTIONS::InstructionType::ADD>;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::ADD_AxEaxRax_imm16imm32_0x5)] = Handle_AxEaxRax_imm16imm32<INSTRUCTIONS::InstructionType::ADD>;

		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::OR_rm8_r8_0x8)] = Handler_rm8_r8<INSTRUCTIONS::InstructionType::OR>;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::OR_rm16rm32_r16r32_0x9)] = Handle_r16r32r64_rm16rm32rm64<INSTRUCTIONS::InstructionType::OR>;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::OR_r8_rm8_0xA)] = Handle_r8_rm8<INSTRUCTIONS::InstructionType::OR>;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::OR_r16r32_rm16rm32_0xB)] = Handle_r16r32r64_rm16rm32rm64<INSTRUCTIONS::InstructionType::OR>;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::OR_AL_imm8_0xC)] = Handle_AL_imm8<INSTRUCTIONS::InstructionType::OR>;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::OR_AxEaxRax_imm16imm32_0xD)] = Handle_AxEaxRax_imm16imm32<INSTRUCTIONS::InstructionType::OR>;

		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::SUB_rm8_r8_0x28)] = Handler_rm8_r8<INSTRUCTIONS::InstructionType::SUB>;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::SUB_rm16rm32_r16r32_0X29)] = Handler_rm16rm32rm64_r16r32r64<INSTRUCTIONS::InstructionType::SUB>;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::SUB_r8_rm8_0x2A)] = Handle_r8_rm8<INSTRUCTIONS::InstructionType::SUB>;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::SUB_r16r32_rm16rm32_0x2B)] = Handle_r16r32r64_rm16rm32rm64<INSTRUCTIONS::InstructionType::SUB>;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::SUB_AL_imm8_0x2C)] = Handle_AL_imm8<INSTRUCTIONS::InstructionType::SUB>;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::SUB_AxEaxRax_imm16imm32_0x2D)] = Handle_AxEaxRax_imm16imm32<INSTRUCTIONS::InstructionType::SUB>;

		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::GROUP1_0X83)] = Handle_GROUP1_0X83_;

		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16AXr32EAX)] = Handle_REX_INCr16AXr32_BASE;//handled partialy
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16CXr32ECX)] = Handle_REX_INCr16AXr32_BASE;//handled partialy
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16DXr32EDX)] = Handle_REX_INCr16AXr32_BASE;//handled partialy
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16BXr32EBX)] = Handle_REX_INCr16AXr32_BASE;//handled partialy
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16SPr32ESP)] = Handle_REX_INCr16AXr32_BASE;//handled partialy
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16BPr32EBP)] = Handle_REX_INCr16AXr32_BASE;//handled partialy
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16SIr32ESI)] = Handle_REX_INCr16AXr32_BASE;//handled partialy
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::REX_INCr16DIr32EDI)] = Handle_REX_INCr16AXr32_BASE;//handled partialy


		//SPECIAL
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16AXr32EAXr64RAXimm16imm32imm64_0xB8)] = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16CXr32ECXr64RCXimm16imm32imm64_0xB9)] = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16DXr32EDXr64RDXimm16imm32imm64_0xBA)] = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16BXr32EBXr64RBXimm16imm32imm64_0xBB)] = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16SPr32ESPr64RSPimm16imm32imm64_0xBC)] = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16BPr32EBPr64RBPimm16imm32imm64_0xBD)] = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16SIr32ESIr64RSIimm16imm32imm64_0xBE)] = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOVr16DIr32EDIr64RDIimm16imm32imm64_0xBF)] = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::MOV_rm16rm32rm64_r16r32r64_0x89)] = Handle_MOV_rm16rm32rm64_r16r32r64_0x89;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::NOP_0x90)] = Handle_NOP_0x90;
		arr[std::to_underlying(INSTRUCTIONS::PrimaryOpcodeByteValue::UD_0xD6)] = Handle_UD_0xD6;
		return arr;
		}();

#pragma warning(suppress: 26440)
	void VirtualCore::decodeInstruction(INSTRUCTIONS::Instruction& instruction)noexcept(false)
	{
		ZoneScoped;//NOLINT
		uint64_t address = RIP.GetValue();

		auto instructionByte = static_cast<uint8_t>(FetchBytes(address, 1));
		address++;
		INSTRUCTIONS::Prefixes prefixes = {};
		while (!HandlerFuncs[instructionByte](*this, address, instruction, prefixes, instructionByte)) {
			instructionByte = static_cast<uint8_t>(FetchBytes(address++, 1));
			//address++;
		}


	}








	[[nodiscard]]
	std::string VirtualCore::getSubregisterFromSize(const REGISTERS::Register* registerPtr, uint8_t bits, bool high) {
		DeepZoneScoped;//NOLINT
		if (registerPtr == &RAX) {
			if (high) {
				return "AH";
			}
			switch (bits) {

				case 8: return "AL"; case 16: return "AX"; case 32:return "EAX";  case 64: default: return "RAX";
			}
		}
		else if (registerPtr == &RBX) {
			if (high) {
				return "BH";
			}
			switch (bits) {
				case 8: return "BL"; case 16: return "BX"; case 32:return "EBX";  case 64: default: return "RBX";
			}
		}
		else if (registerPtr == &RCX) {
			if (high) {
				return "CH";
			}
			switch (bits) {
				case 8: return "CL"; case 16: return "CX"; case 32:return "ECX";  case 64: default: return "RCX";
			}
		}
		else if (registerPtr == &RDX) {
			if (high) {
				return "DH";
			}
			switch (bits) {
				case 8:return "DL";	case 16: return "DX"; case 32:return "EDX";  case 64: default: return "RDX";
			}
		}
		else if (registerPtr ==  &RSI) {
			switch (bits) {
				case 8: return "SIL"; case 16: return "SI"; case 32:return "ESI";  case 64: default: return "RSI";
			}
		}
		else if (registerPtr ==  &RDI) {
			switch (bits) {
				case 8: return "DIL"; case 16: return "DI"; case 32:return "EDI";  case 64: default: return "RDI";
			}
		}
		else if (registerPtr ==  &RSP) {
			switch (bits) {
				case 8: return "SPL"; case 16: return "SP"; case 32:return "ESP";  case 64: default: return "RSP";
			}
		}
		else if (registerPtr ==  &RBP) {
			switch (bits) {
				case 8: return "BPL"; case 16: return "BP"; case 32:return "EBP";  case 64: default: return "RBP";
			}
		}
		else if (registerPtr ==  &R8) {
			switch (bits)
			{
				case 8: return "R8B"; case 16: return "R8W"; case 32:return "R8D";  case 64: default: return "R8";
			}
		}
		else if (registerPtr ==  &R9) {
			switch (bits)
			{
				case 8: return "R9B"; case 16: return "R9W"; case 32:return "R9D";  case 64: default: return "R9";
			}
		}
		else if (registerPtr ==  &R10) {
			switch (bits)
			{
				case 8: return "R10B"; case 16: return "R10W"; case 32:return "R10D";  case 64: default: return "R10";
			}
		}
		else if (registerPtr ==  &R11) {
			switch (bits)
			{
				case 8: return "R11B"; case 16: return "R11W"; case 32:return "R11D";  case 64: default: return "R11";
			}
		}
		else if (registerPtr ==  &R12) {
			switch (bits) {
				case 8: return "R12B"; case 16: return "R12W"; case 32:return "R12D";  case 64: default: return "R12";
			}
		}
		else if (registerPtr ==  &R13) {
			switch (bits) {
				case 8: return "R13B"; case 16: return "R13W"; case 32:return "R13D";  case 64: default: return "R13";
			}
		}
		else if (registerPtr ==  &R14) {
			switch (bits) {
				case 8: return "R14B"; case 16: return "R14W"; case 32:return "R14D";  case 64: default: return "R14";
			}
		}
		else if (registerPtr ==  &R15) {
			switch (bits) {
				case 8: return "R15B"; case 16: return "R15W"; case 32:return "R15D";  case 64: default: return "R15";
			}
		}
		else if (registerPtr ==  &CR0) {
			return "CR0";
		}
		else if (registerPtr ==  &EFER) {
			return "EFER";
		}

		else if (registerPtr == nullptr) {
			return "None";
		}
		else {
			return "Undefined";
		}
	}

	void VirtualCore::WriteBytes(uint64_t address, const uint64_t value, uint8_t sizeBytes)noexcept
	{
		ZoneScoped;//NOLINT
		switch (sizeBytes) {
			case 1: memoryBus->Write8(address, static_cast<uint8_t>(value)); break;
			case 2: memoryBus->Write16(address, static_cast<uint16_t>(value)); break;
			case 4: memoryBus->Write32(address, static_cast<uint32_t>(value)); break;
			case 8: memoryBus->Write64(address, value); break;
			default: NeverOrAssert("size Bytes in " __FUNCTION__ " can only be 1,2,4 or 8");

		}
		
	}

	inline	uint64_t  VirtualCore::FetchBytes(uint64_t address, uint8_t sizeBytes) const noexcept
	{
		ZoneScoped;//NOLINT
		switch (sizeBytes) {
			[[likely]] case 1: return memoryBus->Read8(address);
			case 2: return memoryBus->Read16(address);
			case 4: return memoryBus->Read32(address);
			case 8: return memoryBus->Read64(address);
			default: NeverOrAssert("size Bytes in " __FUNCTION__ " can only be 1,2,4 or 8");
		}
		__assume(false);
	}

	void VirtualCore::PrintInstruction(const INSTRUCTIONS::Instruction& instruction) const
	{
		DeepZoneScoped;
			using namespace INSTRUCTIONS;
		using namespace OPERANDS;
		std::print("Instruction {} {}, {}, {}, {} \n", InstrucionTypeToString(instruction.Type),OperandTypeToString(instruction.Operand0.Type),OperandTypeToString(instruction.Operand1.Type), OperandTypeToString(instruction.Operand2.Type), OperandTypeToString(instruction.Operand3.Type));
		if (instruction.OpcodeSizeBytes == 1) {
			std::print("Raw Opcode : {:#X}\n", instruction.OpcodeBytes[0]);
		}
		else if (instruction.OpcodeSizeBytes == 2) {
			std::print("Raw Opcode: {:#X} {:#X}\n", instruction.OpcodeBytes[0], instruction.OpcodeBytes[1]);
		}
		else {
			std::print("Raw Opcode: {:#X} {:#X} {:#X}\n", instruction.OpcodeBytes[0], instruction.OpcodeBytes[1],instruction.OpcodeBytes[2]);

		}
		std::print("Instructio Length: {}\n", instruction.InstructionLengthBytes);

	}


#pragma warning(suppress: 26440)
	void VirtualCore::executeInstruction(const INSTRUCTIONS::Instruction& instruction)noexcept(false)
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
		try {
		std::print("Core started at reset vector: {:#X}\n", RIP.GetValue());

			while (true) {
				FrameMark;
				RunIfMinimalOrHigherTraceMode(std::print("\nDecoding instruction at RIP: {:#X}\n", RIP.GetValue()););
				 INSTRUCTIONS::Instruction instruction;
				 decodeInstruction(instruction);
				RunIfReducedOrHigherTraceMode(PrintInstruction(instruction));
				RIP.Increment(instruction.InstructionLengthBytes);
				executeInstruction(instruction);
			}
		}
		catch (const EXCEPTIONS::UNDEFINED_OPCODE& e) {

#pragma warning(push)
#pragma warning(disable:26447)
			std::print("{}\n", e.GetMessage());
			PrintCoreState();
#pragma warning(pop)
		}
		hasShutdown.store(true);

		return true;
	}
#pragma warning (push)
#pragma warning (disable:26487)//false positive. alanyzer thinks reference to this outlive function scope
	REGISTERS::Register& VirtualCore::GetRegister(INSTRUCTIONS::TargetRegister reg) noexcept
	{
		DeepZoneScoped;
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
#pragma warning(pop)

}// namespace X86_64_EMU_SOFT::SYSTEM::CPU
