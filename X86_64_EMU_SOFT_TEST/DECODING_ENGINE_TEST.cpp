#include <CppUnitTest.h>

#include "TEST_BOILERPLATE.h"
#include <bit>
#include <cstdint>
#include <memory>
#include <MEMORY.h>
#include <SYSTEM/CPU/DECODING_ENGINE/DECODING_HANDLERS/HANDLERS_ALU.h>
#include <SYSTEM/CPU/DECODING_ENGINE/DECODING_HANDLERS/HANDLERS_SPECIAL.h>
#include <SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h>
#include <SYSTEM/IO_DEVICES/MAIN_MEMORY_DEVICE.h>
#include <VCORE.h>
namespace X86_64_EMU_SOFT::TESTS {
	using namespace X86_64_EMU_SOFT::SYSTEM;
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;

	struct TestEnvironment {
		std::shared_ptr<SYSTEM::MEMORY::MemoryBus> memoryBus;
		std::shared_ptr<SYSTEM::IO_DEVICES::MainMemoryDevice> mainMemoryDevice;
		std::shared_ptr<SYSTEM::CPU::VirtualCore> vCoreReal;
		std::shared_ptr<SYSTEM::CPU::VirtualCore> vCoreProtected;
		std::shared_ptr<SYSTEM::CPU::VirtualCore> vCoreLong;
		TestEnvironment() {
			memoryBus = Boilerplate::CreateMemBus();
			mainMemoryDevice = Boilerplate::CreateMainMemoryDevice(1024);
			memoryBus->MapMainMemory(mainMemoryDevice, 1024 * 1024, 0);
			vCoreLong = Boilerplate::CreateVirtualCore(0, memoryBus, SYSTEM::CPU::vCoreMode::longMode);
			vCoreProtected = Boilerplate::CreateVirtualCore(0, memoryBus, SYSTEM::CPU::vCoreMode::protectedMode);
			vCoreReal = Boilerplate::CreateVirtualCore(0, memoryBus, SYSTEM::CPU::vCoreMode::realMode);
		}
	};

	namespace ALU_DECODING_TESTS
	{



		TEST_CLASS(Test_Handle_ADD_rm8_r8_0x0) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}
			TEST_METHOD(TestRegisterToRegister) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint8_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++;//account for rex prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = false,
					.AddressSizeOverride = false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.W = static_cast<uint8_t>(flags & std::to_underlying(Boilerplate::FlagMask::UseRexW) ? 1U : 0U),
							.reserved = 6U
						}:REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(dst),
						.reg = Boilerplate::TargetRegisterToID(src),
						.mod = 0b11,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));

					bool Handled = Handle_ADD_rm8_r8_0x0(*core, address, instruction, prefixes, 0x0);
					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::ADD), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand1.Data), msg);
					Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits == 8, msg);
					Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits == 8, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(dst)), msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(src)), msg);
					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0x0), msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(2 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);


					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex)|std::to_underlying(Boilerplate::FlagMask::UseRexW), L"Long REXW");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"LONG HIGH");

				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"PROTECTED HIGH");

				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"REAL HIGH");

			}
			TEST_METHOD(TestRegisterToMemory) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint8_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++;//account for rex prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = false,
					.AddressSizeOverride = false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.W = static_cast<uint8_t>(flags & std::to_underlying(Boilerplate::FlagMask::UseRexW) ? 1U : 0U),

							.reserved = 6U
						}:REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(dst),
						.reg = Boilerplate::TargetRegisterToID(src),
						.mod = 0b00,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));
					Assert::ExpectException<EXCEPTIONS::UNDEFINED_OPCODE>([&]() {

						std::ignore = Handle_ADD_rm8_r8_0x0(*core, address, instruction, prefixes, 0x0);
					}, msg);


					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex) | std::to_underlying(Boilerplate::FlagMask::UseRexW), L"Long REXW");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"LONG HIGH");

				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"PROTECTED HIGH");

				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"REAL HIGH");
			}

		};

		TEST_CLASS(Test_Handle_ADD_rm16rm32rm64_r16r32r64_0x1) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}

			TEST_METHOD(TestRegisterToRegister) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++;//account for rex prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride) ? true : false ,
					.AddressSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride) ? true : false ,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.W = static_cast<uint8_t>(flags & std::to_underlying(Boilerplate::FlagMask::UseRexW) ? 1U : 0U),
							.reserved = 6U
						} : REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(dst),
						.reg = Boilerplate::TargetRegisterToID(src),
						.mod = 0b11,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));
					bool Handled = Handle_ADD_rm16rm32rm64_r16r32r64_0x1(*core, address, instruction, prefixes, 0x1);
					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::ADD), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand1.Data), msg);
					uint8_t expectedOperandSize = core->GetDefaultOperandSize();
					if (std::bit_cast<bool>(prefixes.RexPrefix) && prefixes.RexPrefix.W) {
						expectedOperandSize = 64;
					}
					else {
						if (expectedOperandSize == 32 && prefixes.OperandSizeOverride) expectedOperandSize = 16;
						else if (expectedOperandSize == 16 && prefixes.OperandSizeOverride) expectedOperandSize = 32;
					}
					if (expectedOperandSize == 16 && flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride)) {
						
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits, expectedOperandSize, msg);
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits, expectedOperandSize, msg);
					}
					else if (expectedOperandSize == 32 && flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride)) {
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits, expectedOperandSize, msg);
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits, expectedOperandSize, msg);
					}
					else {
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits, expectedOperandSize, msg);
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits, expectedOperandSize, msg);
					}

					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(dst)), msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(src)), msg);
					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0x1), msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(2 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex) | std::to_underlying(Boilerplate::FlagMask::UseRexW), L"Long REXW");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"LONG Operand Size Override");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"LONG Address Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"PROTECTED Operand Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"PROTECTED Address Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"REAL Operand Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"REAL Address Size Override");
			}
			TEST_METHOD(TestRegisterToMemory) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride) ? true : false,
					.AddressSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride) ? true : false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.W = static_cast<uint8_t>(flags & std::to_underlying(Boilerplate::FlagMask::UseRexW) ? 1U : 0U),


							.reserved = 6U
						} : REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(dst),
						.reg = Boilerplate::TargetRegisterToID(src),
						.mod = 0b00,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));
					Assert::ExpectException<EXCEPTIONS::UNDEFINED_OPCODE>([&]() {
						std::ignore = Handle_ADD_rm16rm32rm64_r16r32r64_0x1(*core, address, instruction, prefixes, 0x1);
					}, msg);
					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex) | std::to_underlying(Boilerplate::FlagMask::UseRexW), L"Long REXW");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"LONG Operand Size Override");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"LONG Address Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"PROTECTED Operand Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"PROTECTED Address Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"REAL Operand Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"REAL Address Size Override");

			}
		};

		TEST_CLASS(Test_Handle_ADD_r8_rm8_0x2) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}
			TEST_METHOD(TestRegisterToRegister) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint8_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++;//account for rex prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = false,
					.AddressSizeOverride = false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(dst) >> 3ULL) & 1U),
							.W = static_cast<uint8_t>(flags & std::to_underlying(Boilerplate::FlagMask::UseRexW) ? 1U : 0U),

							.reserved = 6U
						}:REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(src),
						.reg = Boilerplate::TargetRegisterToID(dst),
						.mod = 0b11,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));

					bool Handled = Handle_ADD_r8_rm8_0x2(*core, address, instruction, prefixes, 0x2);
					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::ADD), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand1.Data), msg);
					Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits == 8, msg);
					Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits == 8, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(dst)), msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(src)), msg);
					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0x2), msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(2 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);


					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex) | std::to_underlying(Boilerplate::FlagMask::UseRexW), L"Long REXW");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"LONG HIGH");

				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"PROTECTED HIGH");

				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"REAL HIGH");

			}
			TEST_METHOD(TestRegisterToMemory) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint8_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++;//account for rex prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = false,
					.AddressSizeOverride = false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(dst) >> 3ULL) & 1U),
							.W = 0,
							.reserved = 6U
						}:REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(src),
						.reg = Boilerplate::TargetRegisterToID(dst),
						.mod = 0b00,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));
					Assert::ExpectException<EXCEPTIONS::UNDEFINED_OPCODE>([&]() {

						std::ignore = Handle_ADD_r8_rm8_0x2(*core, address, instruction, prefixes, 0x2);
					}, msg);


					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"LONG HIGH");

				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"PROTECTED HIGH");

				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"REAL HIGH");
			}

		};

		TEST_CLASS(Test_Handle_ADD_r16r32r64_rm16rm32rm64_0x3) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}

			TEST_METHOD(TestRegisterToRegister) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++;//account for rex prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride) ? true : false ,
					.AddressSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride) ? true : false ,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(dst) >> 3ULL) & 1U),
							.W = static_cast<uint8_t>(flags & std::to_underlying(Boilerplate::FlagMask::UseRexW) ? 1U : 0U),
							.reserved = 6U
						} : REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(src),
						.reg = Boilerplate::TargetRegisterToID(dst),
						.mod = 0b11,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));
					bool Handled = Handle_ADD_r16r32r64_rm16rm32rm64_0x3(*core, address, instruction, prefixes, 0x3);
					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::ADD), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand1.Data), msg);
					uint8_t expectedOperandSize = core->GetDefaultOperandSize();
					if (std::bit_cast<bool>(prefixes.RexPrefix) && prefixes.RexPrefix.W) {
						expectedOperandSize = 64;
					}
					else {
						if (expectedOperandSize == 32 && prefixes.OperandSizeOverride) expectedOperandSize = 16;
						else if (expectedOperandSize == 16 && prefixes.OperandSizeOverride) expectedOperandSize = 32;
					}
					if (expectedOperandSize == 16 && flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride)) {

						Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits == expectedOperandSize, msg);
						Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits == expectedOperandSize, msg);
					}
					else if (expectedOperandSize == 32 && flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride)) {
						Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits == expectedOperandSize, msg);
						Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits == expectedOperandSize, msg);
					}
					else {
						Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits == expectedOperandSize, msg);
						Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits == expectedOperandSize, msg);
					}

					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(dst)), msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(src)), msg);
					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0x3), msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(2 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex) | std::to_underlying(Boilerplate::FlagMask::UseRexW), L"Long REXW");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"LONG Operand Size Override");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"LONG Address Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"PROTECTED Operand Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"PROTECTED Address Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"REAL Operand Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"REAL Address Size Override");
			}
			TEST_METHOD(TestRegisterToMemory) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride) ? true : false,
					.AddressSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride) ? true : false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(dst) >> 3ULL) & 1U),
							.W = 0,

							.reserved = 6U
						} : REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(src),
						.reg = Boilerplate::TargetRegisterToID(dst),
						.mod = 0b00,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));
					Assert::ExpectException<EXCEPTIONS::UNDEFINED_OPCODE>([&]() {
						std::ignore = Handle_ADD_r16r32r64_rm16rm32rm64_0x3(*core, address, instruction, prefixes, 0x3);
					}, msg);
					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"LONG Operand Size Override");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"LONG Address Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"PROTECTED Operand Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"PROTECTED Address Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"REAL Operand Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"REAL Address Size Override");

			}
		};
		TEST_CLASS(Test_Handle_ADD_AL_imm8_0x4) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}

			TEST_METHOD(TestImm8ToAL) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;

				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
						.OperandSizeOverride = false,
						.AddressSizeOverride = false,
						.RexPrefix = REX{0,0,0,0,0 }
					};

					// The immediate value that should be fetched and decoded
					uint8_t immediateValue = 0x42;
					env.mainMemoryDevice->Write8(address, immediateValue);

					bool Handled = Handle_ADD_AL_imm8_0x4(*core, address, instruction, prefixes, 0x4);

					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::ADD), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);

					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::ImmediateOperand>(instruction.Operand1.Data), msg);

					auto destOp = std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data);
					auto srcOp = std::get<OPERANDS::ImmediateOperand>(instruction.Operand1.Data);

					Assert::IsTrue(destOp.SizeBits == 8, msg);
					Assert::IsTrue(srcOp.SizeBits == 8, msg);
					Assert::AreEqual(srcOp.Value[0], immediateValue, msg);

					Assert::IsTrue((destOp.Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);

					// Verify AL mapping (RAX pointer with 8 bits operand size limits scope to AL)
					Assert::AreEqual(std::bit_cast<uint64_t>(destOp.RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(INSTRUCTIONS::TargetRegister::RAX)), msg);

					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0x4), msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(2), msg); // 1 byte opcode + 1 byte imm8
					};

				RunTest(env.vCoreLong, L"LONG");
				RunTest(env.vCoreProtected, L"PROTECTED");
				RunTest(env.vCoreReal, L"REAL");
			}
		};




		TEST_CLASS(Test_Handle_ADD_AxEaxRax_imm16imm32_0x5) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}

			TEST_METHOD(TestImmToAxEaxRax) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;

				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};

					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex)) {
						instruction.InstructionLengthBytes++;
					}

					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
						.OperandSizeOverride = (flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride)) != 0,
						.AddressSizeOverride = (flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride)) != 0,
						.RexPrefix = (flags & std::to_underlying(Boilerplate::FlagMask::UseRex)) ?
							REX{
							.B = 0, 
							.X = 0,
							.R = 0,
							.W = static_cast<uint8_t>(flags & std::to_underlying(Boilerplate::FlagMask::UseRexW) ? 1U : 0U),
							.reserved = 0U} : REX{0,0,0,0,0}
					};

					// The handler fetches 2 bytes for 16-bit operation, 4 bytes for 32/64-bit operations.
					env.mainMemoryDevice->Write8(address, 0x11);
					env.mainMemoryDevice->Write8(address + 1, 0x22);
					env.mainMemoryDevice->Write8(address + 2, 0x33);
					env.mainMemoryDevice->Write8(address + 3, 0x44);

					bool Handled = Handle_ADD_AxEaxRax_imm16imm32_0x5(*core, address, instruction, prefixes, 0x5);

					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::ADD), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);

					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::ImmediateOperand>(instruction.Operand1.Data), msg);

					auto destOp = std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data);
					auto srcOp = std::get<OPERANDS::ImmediateOperand>(instruction.Operand1.Data);

					uint8_t expectedOperandSize = core->GetDefaultOperandSize();
					if (std::bit_cast<bool>(prefixes.RexPrefix) && prefixes.RexPrefix.W) {
						expectedOperandSize = 64;
					}
					else {
						if (expectedOperandSize == 32 && prefixes.OperandSizeOverride) expectedOperandSize = 16;
						else if (expectedOperandSize == 16 && prefixes.OperandSizeOverride) expectedOperandSize = 32;
					}

					Assert::AreEqual(destOp.SizeBits, expectedOperandSize, msg);

					if (expectedOperandSize == 16) {
						Assert::AreEqual(srcOp.SizeBits, static_cast<uint8_t>(32), msg);
						Assert::AreEqual(srcOp.Value[0], static_cast<uint8_t>(0x11), msg);
						Assert::AreEqual(srcOp.Value[1], static_cast<uint8_t>(0x22), msg);
						Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(3 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					}
					else if (expectedOperandSize == 32) {
						Assert::AreEqual(srcOp.SizeBits, static_cast<uint8_t>(32), msg);
						Assert::AreEqual(srcOp.Value[0], static_cast<uint8_t>(0x11), msg);
						Assert::AreEqual(srcOp.Value[2], static_cast<uint8_t>(0x33), msg);
						Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(5 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					}
					else if (expectedOperandSize == 64) {
						Assert::AreEqual(srcOp.SizeBits, static_cast<uint8_t>(64), msg);
						Assert::AreEqual(srcOp.Value[0], static_cast<uint8_t>(0x11), msg);
						Assert::AreEqual(srcOp.Value[3], static_cast<uint8_t>(0x44), msg);
						Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(9 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					}

					Assert::IsTrue((destOp.Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::IsTrue((destOp.Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isHighByteRegister)) != 0, msg);

					Assert::AreEqual(std::bit_cast<uint64_t>(destOp.RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(INSTRUCTIONS::TargetRegister::RAX)), msg);

					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0x5), msg);
					};

				RunTest(env.vCoreLong, std::to_underlying(Boilerplate::FlagMask::UseRex), L"LONG REX");
				RunTest(env.vCoreLong, std::to_underlying(Boilerplate::FlagMask::UseRex) | std::to_underlying(Boilerplate::FlagMask::UseRexW), L"LONG REXW");
				RunTest(env.vCoreLong, 0, L"LONG");
				RunTest(env.vCoreLong, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"LONG Operand Size Override");
				RunTest(env.vCoreProtected, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"PROTECTED Operand Size Override");
				RunTest(env.vCoreReal, 0, L"REAL");
				RunTest(env.vCoreReal, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"REAL Operand Size Override");
			}
		};



		TEST_CLASS(Test_Handle_OR_rm8_r8_0x8) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}
			TEST_METHOD(TestRegisterToRegister) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint8_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++; // account for rex prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
						.OperandSizeOverride = false,
						.AddressSizeOverride = false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.W = static_cast<uint8_t>(flags & std::to_underlying(Boilerplate::FlagMask::UseRexW) ? 1U : 0U),
							.reserved = 6U
						} : REX{ 0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(dst),
						.reg = Boilerplate::TargetRegisterToID(src),
						.mod = 0b11,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));

					bool Handled = Handle_OR_rm8_r8_0x8(*core, address, instruction, prefixes, 0x8);
					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::OR), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand1.Data), msg);
					Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits == 8, msg);
					Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits == 8, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(dst)), msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(src)), msg);
					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0x8), msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(2 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					};

				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex) | std::to_underlying(Boilerplate::FlagMask::UseRexW), L"Long REXW");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"LONG HIGH");

				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"PROTECTED HIGH");

				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"REAL HIGH");
			}

			TEST_METHOD(TestRegisterToMemory) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint8_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++; // account for rex prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
						.OperandSizeOverride = false,
						.AddressSizeOverride = false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.W = 0,
							.reserved = 6U
						} : REX{ 0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(dst),
						.reg = Boilerplate::TargetRegisterToID(src),
						.mod = 0b00,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));

					Assert::ExpectException<EXCEPTIONS::UNDEFINED_OPCODE>([&]() {
						std::ignore = Handle_OR_rm8_r8_0x8(*core, address, instruction, prefixes, 0x8);
					}, msg);
					};

				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"LONG HIGH");

				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"PROTECTED HIGH");

				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"REAL HIGH");
			}
		};

		TEST_CLASS(Test_Handle_OR_rm16rm32rm64_r16r32r64_0x9) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}

			TEST_METHOD(TestRegisterToRegister) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++;//account for rex prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride) ? true : false ,
					.AddressSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride) ? true : false ,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(dst) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.W = static_cast<uint8_t>(flags & std::to_underlying(Boilerplate::FlagMask::UseRexW) ? 1U : 0U),
							.reserved = 6U
						} : REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(dst),
						.reg = Boilerplate::TargetRegisterToID(src),
						.mod = 0b11,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));
					bool Handled = Handle_OR_rm16rm32rm64_r16r32r64_0x9(*core, address, instruction, prefixes, 0x9);
					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::OR), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand1.Data), msg);
					uint8_t expectedOperandSize = core->GetDefaultOperandSize();
					if (std::bit_cast<bool>(prefixes.RexPrefix) && prefixes.RexPrefix.W) {
						expectedOperandSize = 64;
					}
					else {
						if (expectedOperandSize == 32 && prefixes.OperandSizeOverride) expectedOperandSize = 16;
						else if (expectedOperandSize == 16 && prefixes.OperandSizeOverride) expectedOperandSize = 32;
					}
					if (expectedOperandSize == 16 && flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride)) {

						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits, expectedOperandSize, msg);
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits, expectedOperandSize, msg);
					}
					else if (expectedOperandSize == 32 && flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride)) {
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits, expectedOperandSize, msg);
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits, expectedOperandSize, msg);
					}
					else {
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits, expectedOperandSize, msg);
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits, expectedOperandSize, msg);
					}

					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(dst)), msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(src)), msg);
					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0x9), msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(2 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex) | std::to_underlying(Boilerplate::FlagMask::UseRexW), L"Long REXW");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"LONG Operand Size Override");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"LONG Address Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"PROTECTED Operand Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"PROTECTED Address Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"REAL Operand Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"REAL Address Size Override");
			}
			TEST_METHOD(TestRegisterToMemory) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride) ? true : false,
					.AddressSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride) ? true : false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(dst) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.W = 0,

							.reserved = 6U
						} : REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(dst),
						.reg = Boilerplate::TargetRegisterToID(src),
						.mod = 0b00,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));
					Assert::ExpectException<EXCEPTIONS::UNDEFINED_OPCODE>([&]() {
						std::ignore = Handle_OR_rm16rm32rm64_r16r32r64_0x9(*core, address, instruction, prefixes, 0x9);
					}, msg);
					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"LONG Operand Size Override");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"LONG Address Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"PROTECTED Operand Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"PROTECTED Address Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"REAL Operand Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"REAL Address Size Override");

			}
		};


		TEST_CLASS(Test_Handle_OR_r8_rm8_0xA) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}
			TEST_METHOD(TestRegisterToRegister) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint8_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++;//account for rex prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = false,
					.AddressSizeOverride = false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(dst) >> 3ULL) & 1U),
							.W = static_cast<uint8_t>(flags & std::to_underlying(Boilerplate::FlagMask::UseRexW) ? 1U : 0U),
							.reserved = 6U
						}:REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(src),
						.reg = Boilerplate::TargetRegisterToID(dst),
						.mod = 0b11,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));

					bool Handled = Handle_OR_r8_rm8_0xA(*core, address, instruction, prefixes, 0x2);
					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::OR), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand1.Data), msg);
					Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits == 8, msg);
					Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits == 8, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(dst)), msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(src)), msg);
					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0x2), msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(2 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);


					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"LONG HIGH");

				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"PROTECTED HIGH");

				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"REAL HIGH");

			}
			TEST_METHOD(TestRegisterToMemory) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint8_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++;//account for rex prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = false,
					.AddressSizeOverride = false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(dst) >> 3ULL) & 1U),
							.W = 0,
							.reserved = 6U
						}:REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(src),
						.reg = Boilerplate::TargetRegisterToID(dst),
						.mod = 0b00,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));
					Assert::ExpectException<EXCEPTIONS::UNDEFINED_OPCODE>([&]() {

						std::ignore = Handle_OR_r8_rm8_0xA(*core, address, instruction, prefixes, 0x2);
					}, msg);


					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"LONG HIGH");

				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"PROTECTED HIGH");

				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"REAL HIGH");
			}

		};

		TEST_CLASS(Test_Handle_OR_r16r32r64_rm16rm32rm64_0xB) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}

			TEST_METHOD(TestRegisterToRegister) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++;//account for rex prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride) ? true : false ,
					.AddressSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride) ? true : false ,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(dst) >> 3ULL) & 1U),
							.W = static_cast<uint8_t>(flags & std::to_underlying(Boilerplate::FlagMask::UseRexW) ? 1U : 0U),
							.reserved = 6U
						} : REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(src),
						.reg = Boilerplate::TargetRegisterToID(dst),
						.mod = 0b11,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));
					bool Handled = Handle_OR_r16r32r64_rm16rm32rm64_0xB(*core, address, instruction, prefixes, 0xB);
					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::OR), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand1.Data), msg);

					uint8_t expectedOperandSize = core->GetDefaultOperandSize();
					if (std::bit_cast<bool>(prefixes.RexPrefix) && prefixes.RexPrefix.W) {
						expectedOperandSize = 64;
					}
					else {
						if (expectedOperandSize == 32 && prefixes.OperandSizeOverride) expectedOperandSize = 16;
						else if (expectedOperandSize == 16 && prefixes.OperandSizeOverride) expectedOperandSize = 32;
					}
					if (expectedOperandSize == 16 && flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride)) {
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits, expectedOperandSize, msg);
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits, expectedOperandSize, msg);
					}
					else if (expectedOperandSize == 32 && flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride)) {
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits, expectedOperandSize, msg);
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits, expectedOperandSize, msg);
					}
					else if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex)) {
						// W bit defaults to 0 in test prefix setup; but if your handler expects W=1 for length 64, it's checked here.
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits, expectedOperandSize, msg);
					}
					else {
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits, expectedOperandSize, msg);
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits, expectedOperandSize, msg);
					}

					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(dst)), msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(src)), msg);
					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0xB), msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(2 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					};

				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex) | std::to_underlying(Boilerplate::FlagMask::UseRexW), L"Long REXW");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"LONG Operand Size Override");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"LONG Address Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"PROTECTED Operand Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"PROTECTED Address Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"REAL Operand Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"REAL Address Size Override");
			}

			TEST_METHOD(TestRegisterToMemory) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride) ? true : false,
					.AddressSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride) ? true : false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(dst) >> 3ULL) & 1U),
							.W = 0,
							.reserved = 6U
						} : REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(src),
						.reg = Boilerplate::TargetRegisterToID(dst),
						.mod = 0b00,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));
					// Current implementation throws an exception for memory operands
					Assert::ExpectException<EXCEPTIONS::UNDEFINED_OPCODE>([&]() {
						std::ignore = Handle_OR_r16r32r64_rm16rm32rm64_0xB(*core, address, instruction, prefixes, 0xB);
					}, msg);
					};

				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"LONG Operand Size Override");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"LONG Address Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"PROTECTED Operand Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"PROTECTED Address Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"REAL Operand Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"REAL Address Size Override");
			}
		};
		TEST_CLASS(Test_Handle_OR_AL_imm8_0xC) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}

			TEST_METHOD(TestImm8ToAL) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;

				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
						.OperandSizeOverride = false,
						.AddressSizeOverride = false,
						.RexPrefix = REX{0,0,0,0,0 }
					};

					// The immediate value that should be fetched and decoded
					uint8_t immediateValue = 0x42;
					env.mainMemoryDevice->Write8(address, immediateValue);

					bool Handled = Handle_OR_AL_imm8_0xC(*core, address, instruction, prefixes, 0xC);

					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::OR), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);

					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::ImmediateOperand>(instruction.Operand1.Data), msg);

					auto destOp = std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data);
					auto srcOp = std::get<OPERANDS::ImmediateOperand>(instruction.Operand1.Data);

					Assert::IsTrue(destOp.SizeBits == 8, msg);
					Assert::IsTrue(srcOp.SizeBits == 8, msg);
					Assert::AreEqual(srcOp.Value[0], immediateValue, msg);

					Assert::IsTrue((destOp.Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);

					// Verify AL mapping (RAX pointer with 8 bits operand size limits scope to AL)
					Assert::AreEqual(std::bit_cast<uint64_t>(destOp.RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(INSTRUCTIONS::TargetRegister::RAX)), msg);

					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0xC), msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(2), msg); // 1 byte opcode + 1 byte imm8
					};

				RunTest(env.vCoreLong, L"LONG");
				RunTest(env.vCoreProtected, L"PROTECTED");
				RunTest(env.vCoreReal, L"REAL");
			}
		};
		TEST_CLASS(Test_Handle_OR_AxEaxRax_imm16imm32_0xD) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}

			TEST_METHOD(TestImmToAxEaxRax) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;

				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};

					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex)) {
						instruction.InstructionLengthBytes++;
					}

					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
						.OperandSizeOverride = (flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride)) != 0,
						.AddressSizeOverride = (flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride)) != 0,
						.RexPrefix = (flags & std::to_underlying(Boilerplate::FlagMask::UseRex)) ?
							REX{
							.B = 0, 
							.X = 0, 
							.R = 0, 
							.W = static_cast<uint8_t>(flags & std::to_underlying(Boilerplate::FlagMask::UseRexW) ? 1U : 0U),
							.reserved = 0U} : REX{0,0,0,0,0}
					};

					// The handler fetches 2 bytes for 16-bit operation, 4 bytes for 32/64-bit operations.
					env.mainMemoryDevice->Write8(address, 0x11);
					env.mainMemoryDevice->Write8(address + 1, 0x22);
					env.mainMemoryDevice->Write8(address + 2, 0x33);
					env.mainMemoryDevice->Write8(address + 3, 0x44);

					bool Handled = Handle_OR_AxEaxRax_imm16imm32_0xD(*core, address, instruction, prefixes, 0xD);

					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::OR), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);

					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::ImmediateOperand>(instruction.Operand1.Data), msg);

					auto destOp = std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data);
					auto srcOp = std::get<OPERANDS::ImmediateOperand>(instruction.Operand1.Data);

					uint8_t expectedOperandSize = core->GetDefaultOperandSize();
					if (std::bit_cast<bool>(prefixes.RexPrefix) && prefixes.RexPrefix.W) {
						expectedOperandSize = 64;
					}
					else {
						if (expectedOperandSize == 32 && prefixes.OperandSizeOverride) expectedOperandSize = 16;
						else if (expectedOperandSize == 16 && prefixes.OperandSizeOverride) expectedOperandSize = 32;
					}

					Assert::AreEqual(destOp.SizeBits, expectedOperandSize, msg);

					if (expectedOperandSize == 16) {
						Assert::AreEqual(srcOp.SizeBits, static_cast<uint8_t>(32), msg);
						Assert::AreEqual(srcOp.Value[0], static_cast<uint8_t>(0x11), msg);
						Assert::AreEqual(srcOp.Value[1], static_cast<uint8_t>(0x22), msg);
						Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(3 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					}
					else if (expectedOperandSize == 32) {
						Assert::AreEqual(srcOp.SizeBits, static_cast<uint8_t>(32), msg);
						Assert::AreEqual(srcOp.Value[0], static_cast<uint8_t>(0x11), msg);
						Assert::AreEqual(srcOp.Value[2], static_cast<uint8_t>(0x33), msg);
						Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(5 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					}
					else if (expectedOperandSize == 64) {
						Assert::AreEqual(srcOp.SizeBits, static_cast<uint8_t>(64), msg);
						Assert::AreEqual(srcOp.Value[0], static_cast<uint8_t>(0x11), msg);
						Assert::AreEqual(srcOp.Value[3], static_cast<uint8_t>(0x44), msg);
						Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(9 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					}

					Assert::IsTrue((destOp.Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::IsTrue((destOp.Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isHighByteRegister)) != 0, msg);

					Assert::AreEqual(std::bit_cast<uint64_t>(destOp.RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(INSTRUCTIONS::TargetRegister::RAX)), msg);

					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0xD), msg);
					};

				RunTest(env.vCoreLong, std::to_underlying(Boilerplate::FlagMask::UseRex), L"LONG REX");
				RunTest(env.vCoreLong, std::to_underlying(Boilerplate::FlagMask::UseRex) | std::to_underlying(Boilerplate::FlagMask::UseRexW), L"LONG REXW");
				RunTest(env.vCoreLong, 0, L"LONG");
				RunTest(env.vCoreLong, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"LONG Operand Size Override");
				RunTest(env.vCoreProtected, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"PROTECTED Operand Size Override");
				RunTest(env.vCoreReal, 0, L"REAL");
				RunTest(env.vCoreReal, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"REAL Operand Size Override");
			}
		};
		TEST_CLASS(Test_Handle_SUB_rm8_r8_0x28) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}

			TEST_METHOD(TestRegisterToRegister) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint8_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++; // account for rex prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
						.OperandSizeOverride = false,
						.AddressSizeOverride = false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.W = 0,
							.reserved = 6U
						} : REX{ 0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(dst),
						.reg = Boilerplate::TargetRegisterToID(src),
						.mod = 0b11,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));

					bool Handled = Handle_SUB_rm8_r8_0x28(*core, address, instruction, prefixes, 0x28);
					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::SUB), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand1.Data), msg);
					Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits == 8, msg);
					Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits == 8, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(dst)), msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(src)), msg);
					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0x28), msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(2 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					};

				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"LONG HIGH");

				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"PROTECTED HIGH");

				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"REAL HIGH");
			}

			TEST_METHOD(TestRegisterToMemory) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint8_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++; // account for rex prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
						.OperandSizeOverride = false,
						.AddressSizeOverride = false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.W = 0,
							.reserved = 6U
						} : REX{ 0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(dst),
						.reg = Boilerplate::TargetRegisterToID(src),
						.mod = 0b00,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));

					// Memory operands are not yet supported, exception expected
					Assert::ExpectException<EXCEPTIONS::UNDEFINED_OPCODE>([&]() {
						std::ignore = Handle_SUB_rm8_r8_0x28(*core, address, instruction, prefixes, 0x28);
					}, msg);
					};

				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"LONG HIGH");

				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"PROTECTED HIGH");

				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"REAL HIGH");
			}
		};
		TEST_CLASS(Test_Handle_SUB_rm16rm32rm64_r16r32r64_0x29) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}

			TEST_METHOD(TestRegisterToRegister) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++; // account for rex prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride) ? true : false,
					.AddressSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride) ? true : false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.W = static_cast<uint8_t>(flags & std::to_underlying(Boilerplate::FlagMask::UseRexW) ? 1U : 0U),
							.reserved = 6U
						} : REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(dst),
						.reg = Boilerplate::TargetRegisterToID(src),
						.mod = 0b11,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));
					bool Handled = Handle_SUB_rm16rm32rm64_r16r32r64_0x29(*core, address, instruction, prefixes, 0x29);
					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::SUB), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand1.Data), msg);
					uint8_t expectedOperandSize = core->GetDefaultOperandSize();
					if (std::bit_cast<bool>(prefixes.RexPrefix) && prefixes.RexPrefix.W) {
						expectedOperandSize = 64;
					}
					else {
						if (expectedOperandSize == 32 && prefixes.OperandSizeOverride) expectedOperandSize = 16;
						else if (expectedOperandSize == 16 && prefixes.OperandSizeOverride) expectedOperandSize = 32;
					}
					if (expectedOperandSize == 16 && flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride)) {
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits, expectedOperandSize, msg);
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits, expectedOperandSize, msg);
					}
					else if (expectedOperandSize == 32 && flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride)) {
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits, expectedOperandSize, msg);
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits, expectedOperandSize, msg);
					}
					else {
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits, expectedOperandSize, msg);
						Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits, expectedOperandSize, msg);
					}

					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(dst)), msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(src)), msg);
					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0x29), msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(2 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex) | std::to_underlying(Boilerplate::FlagMask::UseRexW), L"Long REXW");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"LONG Operand Size Override");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"LONG Address Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"PROTECTED Operand Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"PROTECTED Address Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"REAL Operand Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"REAL Address Size Override");


			}

			TEST_METHOD(TestRegisterToMemory) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride) ? true : false,
					.AddressSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride) ? true : false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.W = 0,
							.reserved = 6U
						} : REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(dst),
						.reg = Boilerplate::TargetRegisterToID(src),
						.mod = 0b00,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));
					Assert::ExpectException<EXCEPTIONS::UNDEFINED_OPCODE>([&]() {
						std::ignore = Handle_SUB_rm16rm32rm64_r16r32r64_0x29(*core, address, instruction, prefixes, 0x29);
					}, msg);
					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"LONG Operand Size Override");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"LONG Address Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"PROTECTED Operand Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"PROTECTED Address Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"REAL Operand Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"REAL Address Size Override");
			}
		};
		TEST_CLASS(Test_Handle_SUB_r8_rm8_0x2A) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}

			TEST_METHOD(TestRegisterToRegister) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint8_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++; // account for REX prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
						.OperandSizeOverride = false,
						.AddressSizeOverride = false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(dst) >> 3ULL) & 1U),
							.W = 0,
							.reserved = 6U
						} : REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(src),
						.reg = Boilerplate::TargetRegisterToID(dst),
						.mod = 0b11,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));

					bool Handled = Handle_SUB_r8_rm8_0x2A(*core, address, instruction, prefixes, 0x2A);

					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::SUB), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand1.Data), msg);
					Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits == 8, msg);
					Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits == 8, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(dst)), msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(src)), msg);
					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0x2A), msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(2 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					};

				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"LONG HIGH");

				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"PROTECTED HIGH");

				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"REAL HIGH");
			}

			TEST_METHOD(TestRegisterToMemory) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint8_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++; // account for REX prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
						.OperandSizeOverride = false,
						.AddressSizeOverride = false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(dst) >> 3ULL) & 1U),
							.W = 0,
							.reserved = 6U
						} : REX{ 0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(src),
						.reg = Boilerplate::TargetRegisterToID(dst),
						.mod = 0b00,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));

					// Expect UNDEFINED_OPCODE exception for unsupported memory addressing
					Assert::ExpectException<EXCEPTIONS::UNDEFINED_OPCODE>([&]() {
						std::ignore = Handle_SUB_r8_rm8_0x2A(*core, address, instruction, prefixes, 0x2A);
					}, msg);
					};

				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"LONG HIGH");

				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"PROTECTED HIGH");

				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::AH, INSTRUCTIONS::TargetRegister::BH, 0, L"REAL HIGH");
			}
		};
		TEST_CLASS(Test_Handle_SUB_r16r32r64_rm16rm32rm64_0x2B) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}

			TEST_METHOD(TestRegisterToRegister) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex))
					{
						instruction.InstructionLengthBytes++; // account for REX prefix
					}
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride) ? true : false ,
					.AddressSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride) ? true : false ,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(dst) >> 3ULL) & 1U),
							.W = 0,
							.reserved = 0U
						} : REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(src),
						.reg = Boilerplate::TargetRegisterToID(dst),
						.mod = 0b11,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));

					bool Handled = Handle_SUB_r16r32r64_rm16rm32rm64_0x2B(*core, address, instruction, prefixes, 0x2B);

					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::SUB), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand1.Data), msg);

					const auto defaultOperandSize = core->GetDefaultOperandSize();
					if (defaultOperandSize == 16 && flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride)) {
						Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits == 32, msg);
						Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits == 32, msg);
					}
					else if (defaultOperandSize == 32 && flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride)) {
						Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits == 16, msg);
						Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits == 16, msg);
					}
					else if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex)) {
						Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits == defaultOperandSize, msg);
					}
					else {
						Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits == defaultOperandSize, msg);
						Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits == defaultOperandSize, msg);
					}

					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(dst)), msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(src)), msg);
					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0x2B), msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(2 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					};

				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"LONG Operand Size Override");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"LONG Address Size Override");

				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"PROTECTED Operand Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"PROTECTED Address Size Override");

				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"REAL Operand Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"REAL Address Size Override");
			}

			TEST_METHOD(TestRegisterToMemory) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride) ? true : false,
					.AddressSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride) ? true : false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(dst) >> 3ULL) & 1U),
							.W = 0,
							.reserved = 0U
						} : REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(src),
						.reg = Boilerplate::TargetRegisterToID(dst),
						.mod = 0b00,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));

					// Verify if the memory operands restriction behaves as intended
					Assert::ExpectException<EXCEPTIONS::UNDEFINED_OPCODE>([&]() {
						std::ignore = Handle_SUB_r16r32r64_rm16rm32rm64_0x2B(*core, address, instruction, prefixes, 0x2B);
					}, msg);
					};

				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
			}
		};
		TEST_CLASS(Test_Handle_SUB_AL_imm8_0x2C) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}

			TEST_METHOD(TestImm8ToAL) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;

				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
						.OperandSizeOverride = false,
						.AddressSizeOverride = false,
						.RexPrefix = REX{0,0,0,0,0 }
					};

					// The immediate value that should be fetched and decoded
					uint8_t immediateValue = 0x42;
					env.mainMemoryDevice->Write8(address, immediateValue);

					bool Handled = Handle_SUB_AL_imm8_0x2C(*core, address, instruction, prefixes, 0x2C);

					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::SUB), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);

					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::ImmediateOperand>(instruction.Operand1.Data), msg);

					auto destOp = std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data);
					auto srcOp = std::get<OPERANDS::ImmediateOperand>(instruction.Operand1.Data);

					Assert::IsTrue(destOp.SizeBits == 8, msg);
					Assert::IsTrue(srcOp.SizeBits == 8, msg);
					Assert::AreEqual(srcOp.Value[0], immediateValue, msg);

					Assert::IsTrue((destOp.Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);

					// Verify AL mapping (RAX pointer with 8 bits operand size limits scope to AL)
					Assert::AreEqual(std::bit_cast<uint64_t>(destOp.RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(INSTRUCTIONS::TargetRegister::RAX)), msg);

					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0x2C), msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(2), msg); // 1 byte opcode + 1 byte imm8
					};

				RunTest(env.vCoreLong, L"LONG");
				RunTest(env.vCoreProtected, L"PROTECTED");
				RunTest(env.vCoreReal, L"REAL");
			}
		};
		TEST_CLASS(Test_Handle_SUB_AxEaxRax_imm16imm32_0x2D) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}

			TEST_METHOD(TestImmToAxEaxRax) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;

				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};

					if (flags & std::to_underlying(Boilerplate::FlagMask::UseRex)) {
						instruction.InstructionLengthBytes++;
					}

					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
						.OperandSizeOverride = (flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride)) != 0,
						.AddressSizeOverride = (flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride)) != 0,
						.RexPrefix = (flags & std::to_underlying(Boilerplate::FlagMask::UseRex)) ?
							REX{.B = 0, .X = 0, .R = 0, .W = 1, .reserved = 0U} : REX{0,0,0,0,0}
					};

					// The handler fetches 2 bytes for 16-bit operation, 4 bytes for 32/64-bit operations.
					env.mainMemoryDevice->Write8(address, 0x11);
					env.mainMemoryDevice->Write8(address + 1, 0x22);
					env.mainMemoryDevice->Write8(address + 2, 0x33);
					env.mainMemoryDevice->Write8(address + 3, 0x44);

					// Using 0x2D byte
					bool Handled = Handle_SUB_AxEaxRax_imm16imm32_0x2D(*core, address, instruction, prefixes, 0x2D);

					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::SUB), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);

					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::ImmediateOperand>(instruction.Operand1.Data), msg);

					auto destOp = std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data);
					auto srcOp = std::get<OPERANDS::ImmediateOperand>(instruction.Operand1.Data);

					uint8_t expectedOperandSize = core->GetDefaultOperandSize();
					if (std::bit_cast<bool>(prefixes.RexPrefix) && prefixes.RexPrefix.W) {
						expectedOperandSize = 64;
					}
					else {
						if (expectedOperandSize == 32 && prefixes.OperandSizeOverride) expectedOperandSize = 16;
						else if (expectedOperandSize == 16 && prefixes.OperandSizeOverride) expectedOperandSize = 32;
					}

					Assert::AreEqual(destOp.SizeBits, expectedOperandSize, msg);

					if (expectedOperandSize == 16) {
						Assert::AreEqual(srcOp.SizeBits, static_cast<uint8_t>(32), msg);
						Assert::AreEqual(srcOp.Value[0], static_cast<uint8_t>(0x11), msg);
						Assert::AreEqual(srcOp.Value[1], static_cast<uint8_t>(0x22), msg);
						Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(3 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					}
					else if (expectedOperandSize == 32) {
						Assert::AreEqual(srcOp.SizeBits, static_cast<uint8_t>(32), msg);
						Assert::AreEqual(srcOp.Value[0], static_cast<uint8_t>(0x11), msg);
						Assert::AreEqual(srcOp.Value[2], static_cast<uint8_t>(0x33), msg);
						Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(5 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					}
					else if (expectedOperandSize == 64) {
						Assert::AreEqual(srcOp.SizeBits, static_cast<uint8_t>(64), msg);
						Assert::AreEqual(srcOp.Value[0], static_cast<uint8_t>(0x11), msg);
						Assert::AreEqual(srcOp.Value[3], static_cast<uint8_t>(0x44), msg);
						Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(9 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					}

					Assert::IsTrue((destOp.Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::IsTrue((destOp.Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isHighByteRegister)) != 0, msg);

					Assert::AreEqual(std::bit_cast<uint64_t>(destOp.RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(INSTRUCTIONS::TargetRegister::RAX)), msg);

					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					// Validating 0x2D byte is parsed as SUB
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0x2D), msg);
					};

				RunTest(env.vCoreLong, std::to_underlying(Boilerplate::FlagMask::UseRex), L"LONG REX");
				RunTest(env.vCoreLong, 0, L"LONG");
				RunTest(env.vCoreLong, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"LONG Operand Size Override");
				RunTest(env.vCoreProtected, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"PROTECTED Operand Size Override");
				RunTest(env.vCoreReal, 0, L"REAL");
				RunTest(env.vCoreReal, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"REAL Operand Size Override");
			}
		};


	}
	namespace SPECIAL_DECODING_TESTS {

		TEST_CLASS(Test_Handle_MOV_rm16rm32rm64_r16r32r64_0x89) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}
			TEST_METHOD(RegisterToRegister) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					instruction.InstructionLengthBytes += flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0; // opcode + ModRM + optional REX
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride) ? true : false,
					.AddressSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride) ? true : false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(dst) >> 3ULL) & 1U),
							.W = 0,
							.reserved = static_cast<uint8_t>(flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 0U : 6U)
						} : REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(dst),
						.reg = Boilerplate::TargetRegisterToID(src),
						.mod = 0b11,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));
					bool Handled = Handle_MOV_rm16rm32rm64_r16r32r64_0x89(*core, address, instruction, prefixes, 0x89);
					Assert::IsTrue(Handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::MOV), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative < OPERANDS::RegisterOperand>(instruction.Operand1.Data), msg);
					Assert::IsTrue(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits == std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).SizeBits, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(dst)), msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand1.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(src)), msg);
					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], static_cast<uint8_t>(0x89), msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(2 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0)), msg);
					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"LONG Operand Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"PROTECTED Operand Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"REAL Operand Size Override");

			}
			TEST_METHOD(RegisterToMemory) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, SYSTEM::CPU::INSTRUCTIONS::TargetRegister src, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride) ? true : false,
					.AddressSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride) ? true : false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(src) >> 3ULL) & 1U),
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToID(dst) >> 3ULL) & 1U),
							.W = 0,
							.reserved = static_cast<uint8_t>(flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 0U : 6U)
						} : REX{0,0,0,0,0 }
					};
					ModRM modrm{
						.rm = Boilerplate::TargetRegisterToID(src),
						.reg = Boilerplate::TargetRegisterToID(dst),
						.mod = 0b00,
					};
					env.mainMemoryDevice->Write8(address, std::bit_cast<uint8_t>(modrm));
					Assert::ExpectException<EXCEPTIONS::UNDEFINED_OPCODE>([&]() {
						std::ignore = Handle_MOV_rm16rm32rm64_r16r32r64_0x89(*core, address, instruction, prefixes, 0x89);
					}, msg);
					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::UseRex) | std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"LONG");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"PROTECTED");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, INSTRUCTIONS::TargetRegister::RBX, std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride), L"REAL");

			}
		};
		TEST_CLASS(Test_Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE) {
			TestEnvironment env;
			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
				Boilerplate::SetRegistersAscending(env.vCoreLong);
				Boilerplate::SetRegistersAscending(env.vCoreProtected);
				Boilerplate::SetRegistersAscending(env.vCoreReal);
			}
			TEST_METHOD(ImmediateToRegister) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;
				auto RunTest = [&](std::shared_ptr<SYSTEM::CPU::VirtualCore> core, SYSTEM::CPU::INSTRUCTIONS::TargetRegister dst, uint16_t flags, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					SYSTEM::CPU::INSTRUCTIONS::Instruction instruction{};
					instruction.InstructionLengthBytes += flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0; // opcode + ModRM + optional REX
					SYSTEM::CPU::INSTRUCTIONS::Prefixes prefixes{
					.OperandSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride) ? true : false,
					.AddressSizeOverride = flags & std::to_underlying(Boilerplate::FlagMask::AddressSizeOverride) ? true : false,
						.RexPrefix = flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? REX{
							.B = 0,
							.X = 0,
							.R = static_cast<uint8_t>((Boilerplate::TargetRegisterToAdditiveID(dst) >> 3ULL) & 1U),
							.W = 0,
							.reserved = static_cast<uint8_t>(flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 0U : 6U)
						} : REX{0,0,0,0,0 }
					};

					std::array <uint8_t, 8> immediateValueBytes{ 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88 };
					for (uint64_t add = address; add < address + immediateValueBytes.size(); add++) {
						env.mainMemoryDevice->Write8(add, immediateValueBytes[add - address]);
					}
					uint8_t expectedOperandSize = core->GetDefaultOperandSize();
					if (std::bit_cast<bool>(prefixes.RexPrefix) && prefixes.RexPrefix.W) {
						expectedOperandSize = 64;
					}
					else {
						if (expectedOperandSize == 32 && prefixes.OperandSizeOverride) expectedOperandSize = 16;
						else if (expectedOperandSize == 16 && prefixes.OperandSizeOverride) expectedOperandSize = 32;
					}


					uint8_t opcode = 0xB8 + (Boilerplate::TargetRegisterToAdditiveID(dst) & 0x7); // opcode is in the range 0xB8-0xBF depending on the target register
					bool handled = Handle_MOV_r16r32r64_imm16_imm32_imm64_0xB8_BASE(*core, address, instruction, prefixes, opcode);
					Assert::IsTrue(handled, msg);
					Assert::AreEqual(std::to_underlying(instruction.Type), std::to_underlying(InstructionType::MOV), msg);
					Assert::AreEqual(instruction.OperandCount, static_cast<uint8_t>(2), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::RegisterOperand>(instruction.Operand0.Data), msg);
					Assert::IsTrue(std::holds_alternative<OPERANDS::ImmediateOperand>(instruction.Operand1.Data), msg);
					Assert::AreEqual(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).SizeBits, expectedOperandSize, msg);
					Assert::AreEqual(std::get<OPERANDS::ImmediateOperand>(instruction.Operand1.Data).SizeBits, expectedOperandSize, msg);
					Assert::IsTrue((std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).Flags & std::to_underlying(OPERANDS::RegisterOperandFlags::isGeneralPurposeRegister)) != 0, msg);
					Assert::AreEqual(std::bit_cast<uint64_t>(std::get<OPERANDS::RegisterOperand>(instruction.Operand0.Data).RegisterPointer), std::bit_cast<uint64_t>(&core->GetRegister(dst)), msg);
					Assert::AreEqual(instruction.OpcodeSizeBytes, static_cast<uint8_t>(1), msg);
					Assert::AreEqual(instruction.OpcodeBytes[0], opcode, msg);
					Assert::AreEqual(instruction.InstructionLengthBytes, static_cast<uint8_t>(1 + (flags & std::to_underlying(Boilerplate::FlagMask::UseRex) ? 1 : 0) + (expectedOperandSize / 8)), msg);
					if (expectedOperandSize == 16) {
						Assert::AreEqual(std::get<OPERANDS::ImmediateOperand>(instruction.Operand1.Data).Value[0], static_cast<uint8_t>(0x11), msg);
						Assert::AreEqual(std::get<OPERANDS::ImmediateOperand>(instruction.Operand1.Data).Value[1], static_cast<uint8_t>(0x22), msg);
					}
					else if (expectedOperandSize == 32) {
						Assert::AreEqual(std::get<OPERANDS::ImmediateOperand>(instruction.Operand1.Data).Value[0], static_cast<uint8_t>(0x11), msg);
						Assert::AreEqual(std::get<OPERANDS::ImmediateOperand>(instruction.Operand1.Data).Value[2], static_cast<uint8_t>(0x33), msg);
					}
					else if (expectedOperandSize == 64) {
						Assert::AreEqual(std::get<OPERANDS::ImmediateOperand>(instruction.Operand1.Data).Value[0], static_cast<uint8_t>(0x11), msg);
						Assert::AreEqual(std::get<OPERANDS::ImmediateOperand>(instruction.Operand1.Data).Value[7], static_cast<uint8_t>(0x88), msg);

					}
					};
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, std::to_underlying(Boilerplate::FlagMask::UseRex), L"Long REX");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, 0, L"LONG");
				RunTest(env.vCoreLong, INSTRUCTIONS::TargetRegister::RAX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"LONG Operand Size Override");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, 0, L"PROTECTED");
				RunTest(env.vCoreProtected, INSTRUCTIONS::TargetRegister::RAX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"PROTECTED Operand Size Override");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, 0, L"REAL");
				RunTest(env.vCoreReal, INSTRUCTIONS::TargetRegister::RAX, std::to_underlying(Boilerplate::FlagMask::OperandSizeOverride), L"REAL Operand Size Override");

			}
		};
		TEST_CLASS(Test_Handle_NOP_0x90) {
			TestEnvironment env;

			TEST_METHOD_INITIALIZE(Init) {
				env = TestEnvironment();
			}

			TEST_METHOD(TestDecodeNOP) {
				using namespace SYSTEM::CPU::INSTRUCTIONS;
				using namespace SYSTEM::CPU;

				auto RunTest = [&](std::shared_ptr<VirtualCore> core, const wchar_t* msg) {
					uint64_t address = 0xFULL;
					Instruction instruction{};
					Prefixes prefixes{};

					uint8_t byte = 0x90;

					// NOP ignores core, address, and prefixes, but we pass them as required by DEFINE_HANDLER.
					bool result = Handle_NOP_0x90(*core, address, instruction, prefixes, byte);

					// Verify the handler successfully finished executing
					Assert::IsTrue(result, msg);

					// Verify instruction state
					Assert::AreEqual(static_cast<uint8_t>(0x90), instruction.OpcodeBytes[0], msg);
					Assert::AreEqual(static_cast<uint8_t>(1), instruction.OpcodeSizeBytes, msg);
					Assert::AreEqual(static_cast<uint8_t>(1), instruction.InstructionLengthBytes, msg);
					Assert::IsTrue(instruction.Type == InstructionType::NOP, msg);
					};

				// Test across all CPU modes
				RunTest(env.vCoreLong, L"LONG");
				RunTest(env.vCoreProtected, L"PROTECTED");
				RunTest(env.vCoreReal, L"REAL");
			}
		};

		TEST_CLASS(Test_Handle_UD_0xD6) {
	public:
		TestEnvironment env;

		TEST_METHOD_INITIALIZE(Init) {
			env = TestEnvironment();
		}

		TEST_METHOD(TestDecoding) {
			using namespace SYSTEM::CPU::INSTRUCTIONS;
			using namespace SYSTEM::CPU;

			auto RunTest = [&](std::shared_ptr<VirtualCore> core, const wchar_t* msg) {
				uint64_t address = 0xFULL;
				Instruction instruction{};
				Prefixes prefixes{};
				uint8_t opcodeByte = 0xD6;

				bool Handled = Handle_UD_0xD6(*core, address, instruction, prefixes, opcodeByte);

				Assert::IsTrue(Handled, msg);
				Assert::AreEqual(std::to_underlying(InstructionType::UD), std::to_underlying(instruction.Type), msg);

				// Ensure that sizes match the single byte undefined operation
				Assert::AreEqual(static_cast<uint8_t>(1), instruction.OpcodeSizeBytes, msg);
				Assert::AreEqual(static_cast<uint8_t>(1), instruction.InstructionLengthBytes, msg);
				Assert::AreEqual(opcodeByte, instruction.OpcodeBytes[0], msg);
				};

			RunTest(env.vCoreLong, L"Long Mode");
			RunTest(env.vCoreProtected, L"Protected Mode");
			RunTest(env.vCoreReal, L"Real Mode");
		}
		};
	}




}