#pragma once
#include <string>
#include <stdexcept>
#include <utility>
namespace X86_64_EMU_SOFT::SYSTEM::CPU::EXCEPTIONS {
	class UNDEFINED_OPCODE:public std::runtime_error {
		std::string message;
	public:
		[[nodiscard]] explicit UNDEFINED_OPCODE(std::string msg):std::runtime_error(msg), message(std::move(msg)) {
		}
		[[nodiscard]] const std::string& GetMessage() const noexcept{
			return message;
		}
	};
}//X86_64_EMU_SOFT::SYSTEM::CPU::EXCEPTIONS