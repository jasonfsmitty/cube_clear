// logging implementation

#include <stdio.h>
#include <cerrno>
#include <cstring>
#include <stdarg.h>
#include <stddef.h>

#include "log.h"
#include "time.h"
#include "defines.h"

namespace {

	bool OpenLog( const std::string& filename );

	struct Data
	{
		int level;

		std::string filename;
		FILE* outFile;

		Data( void )
			: level( LOG_INFO )
			, filename( "log.txt" )
			, outFile( NULL )
		{
			OpenLog( filename );
		}
	};

	Data logData;

	void CloseLog( void )
	{
		if( logData.outFile )
		{
			fclose( logData.outFile );
			logData.outFile = NULL;
		}
	}

	bool OpenLog( const std::string& filename )
	{
		FILE* tmp = fopen( filename.c_str(), "w" );
		if( ! tmp )
		{
			int myerrno = errno;
			logError( "Failed to open new log file '%s': errno=%i [%s]", filename.c_str(), myerrno, strerror(myerrno) );
			return false;
		}

		CloseLog();
		logData.outFile = tmp;
		logData.filename = filename;
		return true;
	}
}

///////////////////////////////////////////////////////////////////////
//
namespace utils {

///////////////////////////////////////////////////////////////////////
const char* LogLevelToString( int level )
{
	static const struct {
		int level;
		const char* str;
	}
	levelMap[] =
	{
		{ LOG_CRIT,   "CRIT" },
		{ LOG_ERR,    "ERROR" },
		{ LOG_INFO,   "INFO" },
		{ LOG_DEBUG,  "DEBUG" },
	};

	for( int i = 0; i < ARRAY_SIZE(levelMap); ++i )
	{
		if( levelMap[i].level == level )
			return levelMap[i].str;
	}
	return "?????";
}

///////////////////////////////////////////////////////////////////////
bool logOpen( const std::string& filename )
{
	return OpenLog( filename );
}

///////////////////////////////////////////////////////////////////////
void logClose( void )
{
	CloseLog();
}

///////////////////////////////////////////////////////////////////////
void logLevel( int level )
{
	logData.level = level;
}

///////////////////////////////////////////////////////////////////////
void logFlush( void )
{
	fflush( stdout );
	fflush( stderr );

	if( logData.outFile )
		fflush( logData.outFile );
}

///////////////////////////////////////////////////////////////////////
void _Log( int level, const char* file, int line, const char* func, const char* fmt, ... )
{
	if( level > logData.level )
		return;

	char mergedFmt[ 512 ];
	va_list ap;

	va_start( ap, fmt );
	vsnprintf( mergedFmt, sizeof(mergedFmt), fmt, ap );
	va_end( ap );

	char timeStr[ 64 ];
	{
		time_t t = utils::TimeInSecs( utils::GetRealTime() );
		struct tm t2;
		localtime_r( &t, &t2 );
		strftime( timeStr, sizeof(timeStr), "%Y%m%d-%H%M%S ", &t2 );
	}

	char prefix[ 256 ];
	{
		char* p = prefix;
#define LEN (sizeof(prefix) - (p - prefix))
		if( 0 ) p += snprintf( p, LEN, "%016llx:", (long long unsigned) 0 ); // logid
		if( 0 ) p += snprintf( p, LEN, "%016llx:", (long long unsigned) pthread_self() ); // tid
		if( 1 ) p += snprintf( p, LEN, "%-6s:",    utils::LogLevelToString( level ) ); // level
		if( 1 ) p += snprintf( p, LEN, "%15s:",   file ); // file
		if( 1 ) p += snprintf( p, LEN, "%5i:",     line ); // line
		if( 0 ) p += snprintf( p, LEN, "%-50s:",   func ); // func
#undef LEN
	}

	fprintf( stdout, "%s%s %s\n", timeStr, prefix, mergedFmt );

	if( logData.outFile )
		fprintf( logData.outFile, "%s%s %s\n", timeStr, prefix, mergedFmt );
}

} // namespace utils

