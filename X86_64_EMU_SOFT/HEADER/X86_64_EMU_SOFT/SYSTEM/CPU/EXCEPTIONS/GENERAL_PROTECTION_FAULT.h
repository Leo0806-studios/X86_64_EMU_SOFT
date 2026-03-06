#pragma once
#include <string>
namespace X86_64_EMU_SOFT::SYSTEM::CPU::EXCEPTIONS
{
			class GENERAL_PROTECTION_FAULT 
			{
				std::string message;
			public:
				explicit GENERAL_PROTECTION_FAULT(const std::string& msg) : message(msg) {}
				[[nodiscard]] const std::string& GetMessage() const noexcept  {
					return message;
				}
			};
}