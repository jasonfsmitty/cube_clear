// Simple asserting macros

#ifndef UTILS_ASSERT_H
#define UTILS_ASSERT_H

#include <signal.h>
#include "log.h"
#include "defines.h"

#define TRAP() \
		do{ \
			logCrit( "*** BREAKPOINT TRIGGERED: returnAddr=%p ***", __builtin_return_address(0) ); \
			utils::logFlush(); \
			raise( SIGTRAP ); \
		} while( 0 )

#define ALWAYS_ASSERT( exp ) \
		do { \
			if( unlikely( !(exp) ) ) { \
				logCrit( "*** ASSERTION FAILED: exp='%s', returnAddr=%p ***", # exp, __builtin_return_address(0) ); \
				utils::logFlush(); \
				raise( SIGTRAP ); \
			} \
		} while( 0 )

#if defined(DEBUG)
#	define ASSERT( exp )   ALWAYS_ASSERT( exp )
#else
#   define ASSERT( exp )   do{}while(0)
#endif

#endif /* UTILS_ASSERT_H */

