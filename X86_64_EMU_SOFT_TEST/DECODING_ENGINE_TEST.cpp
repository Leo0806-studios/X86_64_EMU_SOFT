#include <CppUnitTest.h>

#include <SYSTEM/CPU/DECODING_ENGINE/DECODING_HANDLERS/HANDLERS_ALU.h>
#include <SYSTEM/IO_DEVICES/MAIN_MEMORY_DEVICE.h>
#include <SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h>
#include "TEST_BOILERPLATE.h"

namespace X86_64_EMU_SOFT::TESTS
{
	using namespace X86_64_EMU_SOFT::SYSTEM;
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	TEST_CLASS(ALU_DECODING_TESTS) {
		static std::shared_ptr<SYSTEM::MEMORY::MemoryBus> memoryBus;
		static std::shared_ptr<SYSTEM::IO_DEVICES::MainMemoryDevice> mainMemoryDevice;
		static std::shared_ptr<SYSTEM::CPU::VirtualCore> vCoreReal;
		static std::shared_ptr<SYSTEM::CPU::VirtualCore> vCoreProtected;
		static std::shared_ptr<SYSTEM::CPU::VirtualCore> vCoreLong;
		TEST_CLASS_INITIALIZE(InitAluDecodeTests) {
			memoryBus = Boilerplate::CreateMemBus();
			mainMemoryDevice = Boilerplate::CreateMainMemoryDevice(1024);
			memoryBus->MapMainMemory(mainMemoryDevice, 1024 * 1024, 0);
			vCoreLong = Boilerplate::CreateVirtualCore(0, memoryBus, SYSTEM::CPU::vCoreMode::longMode);
			vCoreProtected = Boilerplate::CreateVirtualCore(0, memoryBus, SYSTEM::CPU::vCoreMode::protectedMode);
			vCoreReal = Boilerplate::CreateVirtualCore(0, memoryBus, SYSTEM::CPU::vCoreMode::realMode);
		}

		TEST_CLASS_CLEANUP(CleanupAluDecodeTests) {
			vCoreReal.reset();
			vCoreProtected.reset();
			vCoreLong.reset();
			memoryBus.reset();
			mainMemoryDevice.reset();
		}

		TEST_METHOD(Test_Handle_ADD_rm8_r8_0x0) {
			auto performRegisterToRegisterTest = [](std::shared_ptr<SYSTEM::CPU::VirtualCore> core) {
				CPU::INSTRUCTIONS::Instruction instruction{};
				CPU::INSTRUCTIONS::Prefixes prefixes{};
				uint64_t address = 0xF;

				// 0xCA -> mod = 3 (11), reg = 1 (001 - RCX), rm = 2 (010 - RDX)
				mainMemoryDevice->Write8(address, 0xCA);

				const bool handled = CPU::Handle_ADD_rm8_r8_0x0(*core, address, instruction, prefixes, 0x0);

				Assert::IsTrue(handled);
				Assert::AreEqual(static_cast<uint16_t>(CPU::INSTRUCTIONS::InstructionType::ADD), static_cast<uint16_t>(instruction.Type));
				Assert::AreEqual(static_cast<uint8_t>(2), instruction.OperandCount);
				Assert::AreEqual(static_cast<uint8_t>(0x0), instruction.OpcodeBytes[0]);
				Assert::AreEqual(static_cast<uint8_t>(1), instruction.OpcodeSizeBytes);

				Assert::AreEqual(static_cast<uint64_t>(0x10), address); // Processed ModRM byte

				Assert::IsTrue(instruction.Operand0.Type == CPU::INSTRUCTIONS::OPERANDS::OperandType::Register);
				Assert::IsTrue(instruction.Operand1.Type == CPU::INSTRUCTIONS::OPERANDS::OperandType::Register);

				auto destOperand = std::get<CPU::INSTRUCTIONS::OPERANDS::RegisterOperand>(instruction.Operand0.Data);
				auto srcOperand = std::get<CPU::INSTRUCTIONS::OPERANDS::RegisterOperand>(instruction.Operand1.Data);

				Assert::AreEqual(static_cast<uint8_t>(8), destOperand.SizeBits);
				Assert::AreEqual(static_cast<uint8_t>(8), srcOperand.SizeBits);

				Assert::IsTrue(destOperand.RegisterPointer == std::bit_cast<std::array<uint8_t, 8>>(&core->GetRegister(CPU::INSTRUCTIONS::TargetRegister::RDX)));
				Assert::IsTrue(srcOperand.RegisterPointer == std::bit_cast<std::array<uint8_t, 8>>(&core->GetRegister(CPU::INSTRUCTIONS::TargetRegister::RCX)));
			};

			auto performMemoryToRegisterTest = [](std::shared_ptr<SYSTEM::CPU::VirtualCore> core) {
				CPU::INSTRUCTIONS::Instruction instruction{};
				CPU::INSTRUCTIONS::Prefixes prefixes{};
				uint64_t address = 0xF;

				// 0x0A -> mod = 0 (00), reg = 1 (001 - RCX), rm = 2 (010 - RDX)
				mainMemoryDevice->Write8(address, 0x0A);

				Assert::ExpectException<CPU::EXCEPTIONS::UNDEFINED_OPCODE>([&] {
					std::ignore = CPU::Handle_ADD_rm8_r8_0x0(*core, address, instruction, prefixes, 0x0);
				});
			};

			// Test Long Mode
			performRegisterToRegisterTest(vCoreLong);
			performMemoryToRegisterTest(vCoreLong);

			// Test Protected Mode
			performRegisterToRegisterTest(vCoreProtected);
			performMemoryToRegisterTest(vCoreProtected);

			// Test Real Mode
			performRegisterToRegisterTest(vCoreReal);
			performMemoryToRegisterTest(vCoreReal);
		}

	};
	std::shared_ptr<class X86_64_EMU_SOFT::SYSTEM::MEMORY::MemoryBus> X86_64_EMU_SOFT::TESTS::ALU_DECODING_TESTS::memoryBus;
	std::shared_ptr<class X86_64_EMU_SOFT::SYSTEM::IO_DEVICES::MainMemoryDevice> X86_64_EMU_SOFT::TESTS::ALU_DECODING_TESTS::mainMemoryDevice;
	std::shared_ptr<class X86_64_EMU_SOFT::SYSTEM::CPU::VirtualCore> X86_64_EMU_SOFT::TESTS::ALU_DECODING_TESTS::vCoreReal;
	std::shared_ptr<class X86_64_EMU_SOFT::SYSTEM::CPU::VirtualCore> X86_64_EMU_SOFT::TESTS::ALU_DECODING_TESTS::vCoreProtected;
		std::shared_ptr<class X86_64_EMU_SOFT::SYSTEM::CPU::VirtualCore> X86_64_EMU_SOFT::TESTS::ALU_DECODING_TESTS::vCoreLong;
}