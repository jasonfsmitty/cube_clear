#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include <list>

class Worker
{
	public:
		enum Status
		{
			Continue,
			Exit,
		};

		virtual Status FrameUpdate( float deltaTime ) = 0;
		virtual Status HandleEvent( const SDL_Event& event ) = 0;
		virtual bool IsAlive( void ) = 0;
};

class MassWorker : public Worker
{
	public:
		MassWorker( void );
		~MassWorker( void );

		void Insert( Worker* worker );
		void Cleanup( bool force );

		virtual Worker::Status FrameUpdate( float deltaTime );
		virtual Worker::Status HandleEvent( const SDL_Event& event );

		virtual bool IsAlive( void ) { return ! m_workers.empty(); }

	private:
		std::list< Worker* > m_workers;
		bool m_dirty;
};

#endif /* GAME_H */

