#ifndef UTILS_LOG_H
#define UTILS_LOG_H

#include <syslog.h>
#include <unistd.h>
#include <string>

namespace utils {

bool logOpen( const std::string& filename );
void logClose( void );

void logFlush( void );

void _Log( int level, const char* file, int line, const char* func, const char* fmt, ... ) __attribute__ ((format (printf, 5, 6 ) ));

#define logDebug( fmt, args... )  utils::_Log( LOG_DEBUG, __FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ## args )
#define logInfo(  fmt, args... )  utils::_Log( LOG_INFO,  __FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ## args )
#define logError( fmt, args... )  utils::_Log( LOG_ERR,   __FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ## args )
#define logCrit(  fmt, args... )  utils::_Log( LOG_CRIT,  __FILE__, __LINE__, __PRETTY_FUNCTION__, fmt, ## args )

} // namespace utils

#endif /* UTILS_LOG_H */

