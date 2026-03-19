#pragma once
namespace X86_64_EMU_SOFT::HELPERS {
	enum class TraceMode {
		none,
		minimal,
		reduced,
		full
	};

	inline TraceMode Tracemode = TraceMode::none;

#define RunIfFullTraceMode(...) if(X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::full) { __VA_ARGS__;}
#define RunIfReducedOrHigherTraceMode(...) if(X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::reduced || X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::full) { __VA_ARGS__;}
#define RunIfMinimalOrHigherTraceMode(...) if(X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::minimal || X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::reduced || X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::full) { __VA_ARGS__;}
}