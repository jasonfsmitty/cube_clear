#ifndef UTILS_DEFINES_H
#define UTILS_DEFINES_H

#include <stdint.h>
#include <stdlib.h>
#include <sstream>

namespace utils {

#define likely( exp )     __builtin_expect( !!(exp), 1 )
#define unlikely( exp )   __builtin_expect( !!(exp), 0 )

#define _QUOTEME_(x)      #x
#define QUOTEME(x)        _QUOTEME_(x)

#define ARRAY_SIZE(x)     (int(sizeof(x) / sizeof(x[0])))

#define Min(x,y)  ((x) < (y) ? (x) : (y))
#define Max(x,y)  ((x) < (y) ? (y) : (y))

inline int64_t KbToBytes( int64_t kb ) { return kb << 10; }
inline int64_t MbToBytes( int64_t mb ) { return mb << 20; }
inline int64_t GbToBytes( int64_t gb ) { return gb << 30; }

inline int64_t BytesToKb( int64_t b ) { return b >> 10; };
inline int64_t BytesToMb( int64_t b ) { return b >> 20; };
inline int64_t BytesToGb( int64_t b ) { return b >> 30; };

inline float randf( void )
{
	return float( rand() ) / float(RAND_MAX);
}

inline float randf( float min, float max )
{
	return (min + randf() * (max - min));
}

template< typename T >
std::string ToString( const T& t )
{
	std::ostringstream oss;
	oss << t;
	return oss.str();
}

} // namespace utils

#endif /* UTILS_DEFINES_H */

