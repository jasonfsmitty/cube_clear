#ifndef GAME_TITLE_H
#define GAME_TITLE_H

#include "worker.h"
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

		virtual Worker::Status Update( float deltaTime )
		{
			m_uptime += deltaTime;
			return m_cubes.Update( deltaTime );
		}

		virtual void Render( void )
		{
			m_cubes.Render();
		}

		virtual Worker::Status Handle( const SDL_Event& event )
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

		virtual void Pause( void )
		{
			m_cubes.Pause();
		}

		virtual void Resume( void )
		{
			m_cubes.Resume();
		}

	private:

		float m_uptime;
		bool m_alive;
		CubeBackground m_cubes;
};

#endif /* GAME_TITLE_H */

