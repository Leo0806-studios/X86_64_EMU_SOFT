#pragma once
#include <boost/program_options.hpp>
namespace X86_64_EMU_SOFT::HELPERS {
	class CmdArgs {
		 boost::program_options::variables_map argMap;
		public:
		[[nodiscard]] explicit CmdArgs(int argc, const char** argv);
		[[nodiscard]] bool Validate() const noexcept;
		[[nodiscard]]const boost::program_options::variables_map& GetArgMap() const noexcept {
			return argMap;
		}
		CmdArgs(const CmdArgs&) = delete;
		CmdArgs& operator=(const CmdArgs&) = delete;
		CmdArgs& operator=(CmdArgs&&) = delete;
		CmdArgs(CmdArgs&&) = delete;
		~CmdArgs() = default;
	};
}// namespace X86_64_EMU_SOFT::HELPERS