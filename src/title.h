#ifndef GAME_TITLE_H
#define GAME_TITLE_H

#include "game.h"

class TitleScreen : public Worker
{
	public:
		TitleScreen( void );
		~TitleScreen( void );

		virtual Worker::Status FrameUpdate( float deltaTime );
		virtual void HandleEvent( const SDL_Event& event );
		virtual bool IsAlive( void );

	private:

		float m_uptime;
		bool m_alive;
};

#endif /* GAME_TITLE_H */

