
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

/////////////////////////////////////////////////////////////////////////////////////////
Timer::Timer( void )
	: m_deltaTime( 0.0f )
	, m_lastUpdateTime( 0LL )
	, m_lastResetTime( 0LL )
{
	Reset();
}

Timer::~Timer( void )
{
	// nothing
}

void Timer::Reset( void )
{
	m_deltaTime = 0.0f;
	m_lastResetTime = m_lastUpdateTime = GetRealTime();
}

float Timer::Update( void )
{
	TimeValue now = GetRealTime();
	m_deltaTime = (float)( double( now - m_lastUpdateTime ) / double(TicksPerSec()) );
	m_lastUpdateTime = now;
	return m_deltaTime;
}

float Timer::Uptime( void )
{
	return (float)( double( GetRealTime() - m_lastResetTime ) / double(TicksPerSec()) );
}


/////////////////////////////////////////////////////////////////////////////////////////


} // namespace

