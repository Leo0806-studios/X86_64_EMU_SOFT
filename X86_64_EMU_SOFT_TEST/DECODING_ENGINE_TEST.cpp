#include <CppUnitTest.h>

#include <SYSTEM/CPU/DECODING_ENGINE/DECODING_HANDLERS/HANDLERS_ALU.h>
#include <SYSTEM/IO_DEVICES/MAIN_MEMORY_DEVICE.h>
#include <SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h>
#include "TEST_BOILERPLATE.h"

namespace X86_64_EMU_SOFT::TESTS::ALU_DECODING_TESTS
{
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

	

	TEST_CLASS(Test_Handle_ADD_rm8_r8_0x0) {

		TestEnvironment env;
		TEST_METHOD_INITIALIZE(Init) {
			env = TestEnvironment();
			Boilerplate::SetRegistersAscending(env.vCoreLong);
			Boilerplate::SetRegistersAscending(env.vCoreProtected);
			Boilerplate::SetRegistersAscending(env.vCoreReal);
		}
		TEST_METHOD(TestRegisterToRegister){
		
		}
		TEST_METHOD(TestRegisterToMemory) {}
		TEST_METHOD(TestREXW){}
		TEST_METHOD(TestPrefixes){}
		TEST_METHOD(TestFaults){}
	};

	TEST_CLASS(Test_Handle_ADD_rm16rm32rm64_r16r32r64_0x1) {

	};

	TEST_CLASS(Test_Handle_ADD_r8_rm8_0x2) {
	
	};

	TEST_CLASS(Test_Handle_ADD_r16r32r64_rm16rm32rm64_0x3) {
	
	};
}
