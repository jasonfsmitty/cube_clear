
#include <unistd.h>

#include "log.h"
#include "sdl.h"

int main( int argc, char* argv[] )
{
	logInfo( "Initializing ..." );

	Sdl sdl;
	SdlDisplay display( sdl );

	if( ! display.Open( 800, 600, 16, false ) )
	{
		logError( "Failed to open display!" );
		return 1;
	}

	logInfo( "Initialization complete." );

	sleep( 5 );

	logInfo( "Exiting ..." );
	return 0;
}
