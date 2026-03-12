#pragma once
#include <atomic>
#include <memory>
#include "SYSTEM/CPU/REGISTERS/GPR.h"
#include "SYSTEM/CPU/REGISTERS/CONTROLL_REGISTERS/CR0.h"
#include "SYSTEM/CPU/REGISTERS/MSR/EFER.h"
#include "SYSTEM/CPU/REGISTERS/RIP.h"
#include "SYSTEM/MEMORY/MEMORY.h"
#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"

namespace X86_64_EMU_SOFT::SYSTEM::CPU
{
		
		
			class VirtualCore
			{
				REGISTERS::GPR RAX;
				REGISTERS::GPR RBX;
				REGISTERS::GPR RCX;
				REGISTERS::GPR RDX;
				REGISTERS::GPR RSI;
				REGISTERS::GPR RDI;
				REGISTERS::GPR RSP;
				REGISTERS::GPR RBP;
				REGISTERS::GPR R8;
				REGISTERS::GPR R9;
				REGISTERS::GPR R10;
				REGISTERS::GPR R11;
				REGISTERS::GPR R12;
				REGISTERS::GPR R13;
				REGISTERS::GPR R14;
				REGISTERS::GPR R15;

				REGISTERS::RIP RIP;
				
				std::atomic_bool isRunning;
				std::atomic_bool hasShutdown;
				std::atomic_bool isEnabled;

				std::shared_ptr<MEMORY::MemoryBus> memoryBus;
				INSTRUCTIONS::Instruction decodeInstruction();
				void executeInstruction(INSTRUCTIONS::Instruction instruction);
				void PrintCoreState()const;
			public:
				explicit VirtualCore(uint64_t resetVector,std::shared_ptr<MEMORY::MemoryBus> memoryBus)noexcept;
				VirtualCore(const VirtualCore&other);
				VirtualCore& operator=(const VirtualCore&other);
				bool hasShutdownCore() const noexcept {
					return hasShutdown.load();
				}

				/// <summary>
				/// the function that starts the core. has to be called via std::thread 
				/// </summary>
				/// <returns></returns>
				bool StartCore() noexcept;

			};
	
}