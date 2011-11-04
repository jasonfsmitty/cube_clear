
#include <unistd.h>

#include "log.h"
#include "sdl.h"
#include "game.h"
#include "title.h"
#include "time.h"

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

	MassWorker root;
	root.Insert( new TitleScreen() );

	utils::Timer timer;
	while( root.IsAlive() && (timer.Uptime() < 20.0f) )
	{
		const float deltaTime = timer.Update();

		SDL_Event event;
		while( SDL_PollEvent( &event ) )
			root.HandleEvent( event );

		root.FrameUpdate( deltaTime );
		SDL_GL_SwapBuffers();

		root.Cleanup( /*force=*/ false );
	}

	logInfo( "Exiting ..." );
	return 0;
}
