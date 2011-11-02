#ifndef SDL_UTILS_H
#define SDL_UTILS_H

#include <SDL/SDL.h>
#include <string>

class Sdl
{
	public:
		Sdl( void )
		{
			SDL_Init( SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_VIDEO );
			SDL_EnableUNICODE( 1 );
		}

		~Sdl( void )
		{
			SDL_Quit();
		}
};

class SdlDisplay
{
	public:
		SdlDisplay( Sdl& sdl );
		~SdlDisplay( void );

		bool Open( int w, int h, int bpp, bool full );
		void Close( void );

		void SetCaption( const std::string& caption );

		SDL_Surface* surface( void ) { return m_surface; }

		bool IsOpen( void ) const { return (m_surface != NULL); }
		bool fullscreen( void ) const { return m_surface->flags & SDL_FULLSCREEN; }
		int  width( void ) const { return m_surface->w; }
		int  height( void ) const { return m_surface->h; }
		int  bpp( void ) const { return m_surface->format->BitsPerPixel; }

	private:
		Sdl& m_sdl;
		SDL_Surface* m_surface;
};


#endif /* SDL_UTILS_H */

