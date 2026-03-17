#pragma once
#include <string>
#include <stdexcept>
#include <utility>
namespace X86_64_EMU_SOFT::SYSTEM::CPU::EXCEPTIONS
{
			class GENERAL_PROTECTION_FAULT :public std::runtime_error
			{
				std::string message;
			public:
				[[nodiscard]] explicit GENERAL_PROTECTION_FAULT(std::string msg) :std::runtime_error(msg), message(std::move(msg)) {
				}
				[[nodiscard]] const std::string& GetMessage() const noexcept  {
					return message;
				}
			};
}//namespace X86_64_EMU_SOFT::SYSTEM::CPU::EXCEPTIONS