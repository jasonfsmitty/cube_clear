#include "game.h"
#include "assert.h"

///////////////////////////////////////////////////////////////////////////////
MassWorker::MassWorker( void )
	: m_workers()
	, m_dirty(false)
{
	// nothing
}

///////////////////////////////////////////////////////////////////////////////
MassWorker::~MassWorker( void )
{
	while( ! m_workers.empty() )
	{
		delete m_workers.front();
		m_workers.pop_front();
	}
}

///////////////////////////////////////////////////////////////////////////////
void MassWorker::Insert( Worker* worker )
{
	ASSERT( worker );
	m_workers.push_back( worker );
}

///////////////////////////////////////////////////////////////////////////////
void MassWorker::Cleanup( bool force )
{
	if( !m_dirty && !force )
		return;

	std::list< Worker* >::iterator iter, tmp;

	iter = m_workers.begin();
	while( iter != m_workers.end() )
	{
		if( (*iter)->IsAlive() )
			iter++;
		else
		{
			tmp = iter++;
			delete *tmp;
			m_workers.erase( tmp );
		}
	}

	m_dirty = false;
}

///////////////////////////////////////////////////////////////////////////////
Worker::Status MassWorker::FrameUpdate( float deltaTime )
{
	std::list< Worker* >::iterator iter;

	for( iter = m_workers.begin(); iter != m_workers.end(); iter++ )
	{
		Worker* worker = *iter;
		if( ! worker->IsAlive() )
		{
			m_dirty = true;
			continue;
		}

		Worker::Status status = worker->FrameUpdate( deltaTime );
		if( status != Worker::Continue )
			return status;
	}

	return Worker::Continue;
}

///////////////////////////////////////////////////////////////////////////////
Worker::Status MassWorker::HandleEvent( const SDL_Event& event )
{
	std::list< Worker* >::iterator iter;

	for( iter = m_workers.begin(); iter != m_workers.end(); iter++ )
	{
		Worker* worker = *iter;
		if( ! worker->IsAlive() )
		{
			m_dirty = true;
			continue;
		}

		Worker::Status status = worker->HandleEvent( event );
		if( status != Worker::Continue )
			return status;
	}

	return Worker::Continue;
}

///////////////////////////////////////////////////////////////////////////////

