#pragma once
#include <string>
#include <stdexcept>
namespace X86_64_EMU_SOFT::SYSTEM::CPU::EXCEPTIONS {
	class UNDEFINED_OPCODE:public std::runtime_error {
		std::string msg;
	public:
		[[nodiscard]] explicit UNDEFINED_OPCODE(std::string msg):std::runtime_error(msg), msg(std::move(msg)) {
		}
		[[nodiscard]] const std::string& GetMessage() const noexcept{
			return msg;
		}
	};
}