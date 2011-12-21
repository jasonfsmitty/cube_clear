#ifndef GAME_TITLE_H
#define GAME_TITLE_H

#include "worker.h"
#include "background.h"
#include "game.h"
#include "font.h"
#include "menu.h"

class TitleScreen : public Worker
{
	public:
		TitleScreen( void );
		~TitleScreen( void );

		virtual Worker::Status Handle( const SDL_Event& event );
		virtual Worker::Status Update( float deltaTime );
		virtual void Render( void );

		virtual void Pause( void );
		virtual void Resume( void );

		virtual bool IsAlive( void ) { return m_alive; }

	private:

		void RenderTitle( void );
		void RenderMenu( void );

		Worker::Status HandleTitleEvent( const SDL_Event& event );
		Worker::Status HandleGameEvent( const SDL_Event& event );
		Worker::Status HandleMenuEvent( const SDL_Event& event );

		void GotoTitleState( void );
		void GotoGameState( void );
		void GotoMenuState( void );

		enum State
		{
			TITLE,
			GAME,
			MENU,
		};

		float m_uptime;
		bool m_alive;
		State m_state;

		CubeBackground m_cubes;
		Game m_game;
		GlFont m_font;
		Menu m_menu;
};

#endif /* GAME_TITLE_H */

