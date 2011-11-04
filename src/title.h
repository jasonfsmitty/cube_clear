#ifndef GAME_TITLE_H
#define GAME_TITLE_H

#include "game.h"
#include "background.h"

#include <GL/gl.h>

class TitleScreen : public Worker
{
	public:
		TitleScreen( void )
			: m_uptime( 0.0f )
			, m_alive( true )
			, m_cubes()
		{
			// nothing
		}

		~TitleScreen( void )
		{
			// nothing
		}

		virtual Worker::Status FrameUpdate( float deltaTime )
		{
			m_uptime += deltaTime;

			if( m_uptime >= 10.0f )
			{
				logInfo( "Title screen reached max time, exiting" );
				m_alive = false;
			}

			glClear( GL_COLOR_BUFFER_BIT );
			return m_cubes.FrameUpdate( deltaTime );
		}

		virtual Worker::Status HandleEvent( const SDL_Event& event )
		{
			switch( event.type )
			{
				case SDL_QUIT:
					m_alive = false;
					return Worker::Exit;

				case SDL_KEYDOWN:
					if( event.key.keysym.sym == SDLK_ESCAPE )
					{
						m_alive = false;
						return Worker::Exit;
					}

				default:
					break;
			}
			return Worker::Continue;
		}

		virtual bool IsAlive( void ) { return m_alive; }

	private:

		float m_uptime;
		bool m_alive;
		CubeBackground m_cubes;
};

#endif /* GAME_TITLE_H */

