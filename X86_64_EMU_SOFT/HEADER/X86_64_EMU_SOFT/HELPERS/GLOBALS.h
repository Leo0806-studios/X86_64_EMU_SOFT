#pragma once
namespace X86_64_EMU_SOFT::HELPERS {
	enum class TraceMode {
		none,
		minimal,
		reduced,
		full
	};

	inline TraceMode Tracemode = TraceMode::none;


}// namespace X86_64_EMU_SOFT::HELPERS