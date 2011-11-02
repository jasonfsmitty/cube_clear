
#include "time.h"
#include "log.h"

#include <cerrno>
#include <cstring>

namespace utils {

/////////////////////////////////////////////////////////////////////////////////////////
TimeValue GetTime( clockid_t id )
{
	struct timespec ts;

	if( clock_gettime( id, &ts ) )
	{
		int myerrno = errno;
		logError( "Failed to get time: errno=%i [%s]", myerrno, strerror(myerrno) );
	}

	return ( TimeValue(ts.tv_sec) * 1000000LL) + ( TimeValue(ts.tv_nsec) / 1000LL );
}

} // namespace

