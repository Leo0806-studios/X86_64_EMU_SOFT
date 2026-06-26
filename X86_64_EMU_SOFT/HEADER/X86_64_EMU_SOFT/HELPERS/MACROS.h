#pragma once
#include <source_location>
#include <tracy/Tracy.hpp>
#include "GLOBALS.h"
#define RunIfFullTraceMode(...) if(X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::full) { __VA_ARGS__;}(void)nullptr
#define RunIfReducedOrHigherTraceMode(...) if(X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::reduced || X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::full) { __VA_ARGS__;}(void)nullptr
#define RunIfMinimalOrHigherTraceMode(...) if(X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::minimal || X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::reduced || X86_64_EMU_SOFT::HELPERS::Tracemode == X86_64_EMU_SOFT::HELPERS::TraceMode::full) { __VA_ARGS__;} (void)nullptr


#pragma warning(push)
#pragma warning(disable:4005)
#ifdef TRACY_ENABLE	
#	define ZoneNamed( varname, active ) \
__pragma(warning(push,0))\
__pragma(warning(disable:26447))\
static constexpr tracy::SourceLocationData __tracy_source_location { nullptr, TracyFunction,  TracyFile, (uint32_t)std::source_location::current().line(), 0 }; tracy::ScopedZone varname( &__tracy_source_location, active )\
__pragma(warning(pop))
#	define ZoneScoped \
__pragma(warning(push,0))\
__pragma(warning(disable:26447))\
 ZoneNamed( ___tracy_scoped_zone, true )\
__pragma(warning(pop))


#	ifdef DEEP_PROFILING
#		define DeepZoneScoped  \
__pragma(warning(push,0))\
__pragma(warning(disable:26447))\
ZoneScoped; ZoneColor(0xff00ff)\
__pragma(warning(pop))
#	else
#		define DeepZoneScoped
#	endif
#else
#	define DeepZoneScoped
#endif
#	pragma warning(pop)


#ifdef _DEBUG
#	include <cassert>
#	define NeverOrAssert(...) assert(__VA_ARGS__)
#else
#	define NeverOrAssert(...) __assume(false)
#endif 