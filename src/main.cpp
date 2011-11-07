
#include <unistd.h>

#include "log.h"
#include "sdl.h"
#include "worker.h"
#include "title.h"
#include "time.h"

#include <cstdlib>

int main( int argc, char* argv[] )
{
	utils::logLevel( LOG_DEBUG );
	logInfo( "Initializing ..." );

	srand( (unsigned int) utils::GetRealTime() );

	Sdl sdl;
	SdlDisplay display( sdl );

	if( ! display.Open( 800, 600, 16, false ) )
	{
		logError( "Failed to open display!" );
		return 1;
	}

	logInfo( "Initialization complete." );

	MassWorker root;
	root.Insert( new TitleScreen() );
	root.Resume();

	utils::Timer timer;
	while( root.IsAlive() )
	{
		const float deltaTime = timer.Update();

		SDL_Event event;
		while( SDL_PollEvent( &event ) )
			root.Handle( event );

		root.Update( deltaTime );

		glClear( GL_COLOR_BUFFER_BIT );
		root.Render();
		SDL_GL_SwapBuffers();

		root.Cleanup( /*force=*/ false );
	}

	logInfo( "Exiting ..." );
	return 0;
}
