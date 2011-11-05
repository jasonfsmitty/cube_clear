#ifndef GAME_H
#define GAME_H

#include "worker.h"
#include "board.h"

class Game : public Worker
{
	public:
		Game( Worker& background );
		virtual ~Game( void );

		virtual Status Update( float deltaTime );
		virtual Status Handle( const SDL_Event& event );
		virtual void Render( void );

		virtual bool IsAlive( void ) { return m_alive; }

		virtual void Pause( void );
		virtual void Resume( void );

	private:

		Worker& m_background;
		Board m_board;
		bool m_alive;
};

#endif /* GAME_H */
