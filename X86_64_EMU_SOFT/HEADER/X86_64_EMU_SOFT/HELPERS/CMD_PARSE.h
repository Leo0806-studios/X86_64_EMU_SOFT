#pragma once
#include <boost/program_options.hpp>
namespace X86_64_EMU_SOFT::HELPERS {
	class CmdArgs {
		 boost::program_options::variables_map ArgMap;
		public:
		[[nodiscard]] explicit CmdArgs(int argc, const char** argv);
		[[nodiscard]] bool Validate() const noexcept;
		const boost::program_options::variables_map& GetArgMap() const noexcept {
			return ArgMap;
		}
		CmdArgs(const CmdArgs&) = delete;
		CmdArgs& operator=(const CmdArgs&) = delete;
		CmdArgs& operator=(CmdArgs&&) = delete;
		CmdArgs(CmdArgs&&) = delete;
	};
}