
#include "sdl.h"
#include "log.h"
#include "assert.h"

#include <GL/gl.h>

/////////////////////////////////////////////////////////////////////////
SdlDisplay::SdlDisplay( Sdl& sdl )
	: m_sdl( sdl )
	, m_surface( NULL )
{
	// nothing
}

/////////////////////////////////////////////////////////////////////////
SdlDisplay::~SdlDisplay( void )
{
	Close();
}

/////////////////////////////////////////////////////////////////////////
bool SdlDisplay::Open( int w, int h, int bpp, bool full )
{
	logInfo( "Opening display: w=%i h=%i bpp=%i full=%i", w, h, bpp, full );
	ASSERT( ! m_surface );
	if( m_surface )
	{
		logError( "Cannot open a display that is already opened!" );
		return false;
	}

	if( ! SDL_WasInit( SDL_INIT_VIDEO ) )
	{
		if( SDL_InitSubSystem( SDL_INIT_VIDEO ) < 0 )
		{
			logError( "Failed to initialize video subsystem, cannot open display: %s", SDL_GetError() );
			return false;
		}
		logDebug( "Video subsystem initialized" );
	}

	const SDL_VideoInfo* info = SDL_GetVideoInfo();
	if( ! info )
	{
		logError( "Failed to query video info: %s", SDL_GetError() );
		SDL_QuitSubSystem( SDL_INIT_VIDEO );
		return false;
	}

	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, bpp/3 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, bpp/3 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, bpp/3 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 ); // depth buffer size

	int flags = SDL_OPENGL;
	if( full )
		flags |= SDL_FULLSCREEN;

	m_surface = SDL_SetVideoMode( w, h, bpp, flags );
	if( ! m_surface )
	{
		logError( "Failed to set video mode: w=%i h=%i bpp=%i flags=%i error=%s", w, h, bpp, flags, SDL_GetError() );
		SDL_QuitSubSystem( SDL_INIT_VIDEO );
		return false;
	}

	char temp[ 128 ]={0};
	logInfo( "Video initialized.  SDL reports driver as: %s", SDL_VideoDriverName(temp, sizeof(temp) ) );

	logInfo( "OpenGL information:" );
	logInfo( "  GL_VENDOR     : %s", glGetString( GL_VENDOR ) );
	logInfo( "  GL_RENDERER   : %s", glGetString( GL_RENDERER ) );
	logInfo( "  GL_VERSION    : %s", glGetString( GL_VERSION ) );
	logInfo( "  GL_EXTENSIONS : %s", glGetString( GL_EXTENSIONS ) );

	return true;
}

/////////////////////////////////////////////////////////////////////////
void SdlDisplay::SetCaption( const std::string& caption )
{
	SDL_WM_SetCaption( caption.c_str(), NULL );
}

/////////////////////////////////////////////////////////////////////////
void SdlDisplay::Close( void )
{
	if( m_surface )
	{
		SDL_QuitSubSystem( SDL_INIT_VIDEO );
		m_surface = NULL;
	}
}


