//#include "SYSTEM/CPU/INSTRUCTIONS/INSTRUCTION.h"
//#include <SYSTEM/CPU/DECODING_ENGINE/DECODING_ENGINE.h>
//namespace X86_64_EMU_SOFT::SYSTEM::CPU::INSTRUCTIONS::OPERANDS {
//#ifdef _DEBUG
//	[[nodiscard]]
//	inline RegisterOperand::RegisterOperand(uint8_t regSelector, uint8_t sizeBits, uint8_t flags) noexcept {
//		assert(sizeBits == 8 || sizeBits == 16 || sizeBits == 32 || sizeBits == 64);
//		if (sizeBits == 8) {
//			//handle 8 bit registers
//
//		}
//		else {
//			const INSTRUCTIONS::TargetRegister targetRegister = DecodingEngine::DecodeTargetRegister(regSelector);
//			RegisterPointer=std::bit_cast<std::array<uint8_t, 8>>(&core.GetRegister(sourceRegister))
//		}
//	}
//
//#else
//
//#endif // _DEBUG
//
//}
