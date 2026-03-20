#pragma once
#include "GLOBALS.h"
#define RunIfFullTraceMode(...) if(X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::full) { __VA_ARGS__;}
#define RunIfReducedOrHigherTraceMode(...) if(X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::reduced || X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::full) { __VA_ARGS__;}
#define RunIfMinimalOrHigherTraceMode(...) if(X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::minimal || X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::reduced || X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::full) { __VA_ARGS__;}
#ifdef __TRACY_HPP__
#include "HELPERS/REDEFINE_MACROS.h"
#ifdef DEEP_PROFILING
#define DeepZoneScoped ZoneScoped; ZoneColor(0xff00ff)
#else
#define DeepZoneScoped
#endif
#endif