#pragma once
#include <source_location>
#include <tracy/Tracy.hpp>
#include "GLOBALS.h"
#define RunIfFullTraceMode(...) if(X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::full) { __VA_ARGS__;}
#define RunIfReducedOrHigherTraceMode(...) if(X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::reduced || X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::full) { __VA_ARGS__;}
#define RunIfMinimalOrHigherTraceMode(...) if(X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::minimal || X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::reduced || X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::full) { __VA_ARGS__;}


#pragma warning(push)
#pragma warning(disable:4005)
# define ZoneNamed( varname, active ) static constexpr tracy::SourceLocationData __tracy_source_location { nullptr, TracyFunction,  TracyFile, (uint32_t)std::source_location::current().line(), 0 }; tracy::ScopedZone varname( &__tracy_source_location, active )
#define ZoneScoped ZoneNamed( ___tracy_scoped_zone, true )
#pragma warning(pop)


#ifdef DEEP_PROFILING
#define DeepZoneScoped ZoneScoped; ZoneColor(0xff00ff)
#else
#define DeepZoneScoped
#endif


#ifdef _DEBUG
#include <cassert>
#define NeverOrAssert(...) assert(__VA_ARGS__)
#else
#define NeverOrAssert(...) __assume(false)
#endif 