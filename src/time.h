#ifndef	UTILS_TIME_H
#define UTILS_TIME_H

#include <time.h>
#include <stdint.h>

namespace utils {

// Define holder for our time value, representing microseconds.
typedef uint64_t  TimeValue;

// See man clock_gettime() for more info.
TimeValue GetTime( clockid_t id );

// Convenience routines
inline TimeValue GetRealTime( void ) { return GetTime(CLOCK_REALTIME); };
inline TimeValue GetMonoTime( void ) { return GetTime(CLOCK_MONOTONIC); };

inline TimeValue TicksPerSec( void )         { return 1000000LL; };
inline TimeValue TimeInSecs ( TimeValue tv ) { return (tv / TicksPerSec()); };
inline TimeValue TimeInMSecs( TimeValue tv ) { return (tv % TicksPerSec()); };

} // namespace

#endif /* UTILS_TIME_H */
