#pragma once
#include <string>

namespace X86_64_EMU_SOFT::SYSTEM::CPU::EXCEPTIONS {
	class UNDEFINED_OPCODE {
		std::string msg;
	public:
		[[nodiscard]] explicit UNDEFINED_OPCODE(std::string msg):msg(std::move(msg)){ }
		[[nodiscard]] const std::string& GetMessage() const {
			return msg;
		}
	};
}