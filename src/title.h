#ifndef GAME_TITLE_H
#define GAME_TITLE_H

#include "worker.h"
#include "background.h"
#include "game.h"
#include "font.h"

#include <GL/gl.h>

class TitleScreen : public Worker
{
	public:
		TitleScreen( void )
			: m_uptime( 0.0f )
			, m_alive( true )
			, m_state( TITLE )
			, m_cubes()
			, m_game( m_cubes )
			, m_font()
		{
			GotoTitleState();
			m_font.Load( "VeraBd.ttf", 16 );
		}

		~TitleScreen( void )
		{
			// nothing
		}

		void GotoTitleState( void )
		{
			m_state = TITLE;
			m_game.Reset();
		}

		void GotoGameState( void )
		{
			m_state = GAME;
			m_game.Resume();
		}

		virtual Worker::Status Update( float deltaTime )
		{
			m_uptime += deltaTime;
			switch( m_state )
			{
				case TITLE:
					m_cubes.Update( deltaTime );
					break;

				case GAME:
					if( Worker::Continue != m_game.Update( deltaTime ) )
						GotoTitleState();
					break;
			}
			return Worker::Continue;
		}

		virtual void Render( void )
		{
			switch( m_state )
			{
				case TITLE:
					RenderTitle();
					break;

				case GAME:
					m_game.Render();
					break;
			}
		}

		void RenderTitle( void )
		{
			m_cubes.Render();

			glLoadIdentity();
			glTranslatef( 0.0f, 0.0f, -1.0f );

			GlLayout layout;
			GlPrinter printer( m_font );
			glColor4f( 1.0f, 1.0f, 1.0f, 0.9f );
			printer.Print( 10, m_font.height(), "Hello World" );
		}

		Worker::Status HandleTitleEvent( const SDL_Event& event )
		{
			switch( event.type )
			{
				case SDL_QUIT:
					m_alive = false;
					return Worker::Exit;

				case SDL_KEYDOWN:
					switch( event.key.keysym.sym )
					{
						case SDLK_RETURN:
						case SDLK_KP_ENTER:
							GotoGameState();
							return Worker::Continue;

						case SDLK_ESCAPE:
							m_alive = false;
							return Worker::Exit;

						default:
							break;
					}
					break;

				default:
					break;
			}
			return Worker::Continue;
		}

		Worker::Status HandleGameEvent( const SDL_Event& event )
		{
			if( Worker::Continue != m_game.Handle( event ) )
				GotoTitleState();

			return Worker::Continue;
		}

		virtual Worker::Status Handle( const SDL_Event& event )
		{
			switch( m_state )
			{
				case TITLE:
					return HandleTitleEvent( event );

				case GAME:
					return HandleGameEvent( event );
			}

			return Worker::Exit;
		}

		virtual bool IsAlive( void ) { return m_alive; }

		virtual void Pause( void )
		{
			switch( m_state )
			{
				case TITLE:
					m_cubes.Pause();
					break;
				case GAME:
					m_game.Pause();
					break;
			}
		}

		virtual void Resume( void )
		{
			switch( m_state )
			{
				case TITLE:
					m_cubes.Resume();
					break;
				case GAME:
					m_game.Resume();
					break;
			}
		}

	private:

		enum State
		{
			TITLE,
			GAME,
		};

		float m_uptime;
		bool m_alive;
		State m_state;

		CubeBackground m_cubes;
		Game m_game;
		GlFont m_font;
};

#endif /* GAME_TITLE_H */

