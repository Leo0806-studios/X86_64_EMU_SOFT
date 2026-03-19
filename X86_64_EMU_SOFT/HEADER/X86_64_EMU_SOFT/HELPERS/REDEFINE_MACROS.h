#pragma once
#include <source_location>
#pragma warning(push)
#pragma warning(disable:4005)
#ifdef ZoneNamed
# define ZoneNamed( varname, active ) static constexpr tracy::SourceLocationData __tracy_source_location { nullptr, TracyFunction,  TracyFile, (uint32_t)std::source_location::current().line(), 0 }; tracy::ScopedZone varname( &__tracy_source_location, active )
#define ZoneScoped ZoneNamed( ___tracy_scoped_zone, true )
#endif
#pragma warning(pop)