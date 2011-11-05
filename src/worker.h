#ifndef WORKER_H
#define WORKER_H

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

		virtual Status Update( float deltaTime ) = 0;
		virtual Status Handle( const SDL_Event& event ) = 0;
		virtual void   Render( void ) = 0;

		virtual bool IsAlive( void ) = 0;

		virtual void Pause( void ) = 0;
		virtual void Resume( void ) = 0;
};

class LazyWorker : public Worker
{
	public:
		LazyWorker( void ) {}
		virtual ~LazyWorker( void ) {}

		virtual Status Update( float deltaTime )
			{ return Continue; }

		virtual Status Handle( const SDL_Event& event )
			{ return Continue; }

		virtual void Render( void )
			{}

		virtual bool IsAlive( void )
			{ return true; }

		virtual void Pause( void )
			{}

		virtual void Resume( void )
			{}
};

class MassWorker : public Worker
{
	public:
		MassWorker( void );
		~MassWorker( void );

		void Insert( Worker* worker );
		void Cleanup( bool force );

		virtual Worker::Status Update( float deltaTime );
		virtual Worker::Status Handle( const SDL_Event& event );
		virtual void Render( void );

		virtual bool IsAlive( void ) { return ! m_workers.empty(); }

		virtual void Pause( void );
		virtual void Resume( void );

	private:
		struct WorkerInfo
		{
			Worker* worker;
			bool alive;

			WorkerInfo( Worker* w )
				: worker(w), alive(true) {}

			void Kill( void ) { alive=false; }
			bool IsAlive( void ) { return worker->IsAlive() && alive; }
		};

		std::list< WorkerInfo > m_workers;
		bool m_dirty;
};

#endif /* WORKER_H */

