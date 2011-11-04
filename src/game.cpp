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
		delete m_workers.front().worker;
		m_workers.pop_front();
	}
}

///////////////////////////////////////////////////////////////////////////////
void MassWorker::Insert( Worker* worker )
{
	ASSERT( worker );
	m_workers.push_back( WorkerInfo( worker ) );
}

///////////////////////////////////////////////////////////////////////////////
void MassWorker::Cleanup( bool force )
{
	if( !m_dirty && !force )
		return;

	std::list< WorkerInfo >::iterator iter, tmp;

	iter = m_workers.begin();
	while( iter != m_workers.end() )
	{
		if( (*iter).IsAlive() )
			iter++;
		else
		{
			tmp = iter++;
			delete (*tmp).worker;
			m_workers.erase( tmp );
		}
	}

	m_dirty = false;
}

///////////////////////////////////////////////////////////////////////////////
Worker::Status MassWorker::FrameUpdate( float deltaTime )
{
	std::list< WorkerInfo >::iterator iter;

	for( iter = m_workers.begin(); iter != m_workers.end(); iter++ )
	{
		WorkerInfo& info = *iter;
		if( ! info.IsAlive() )
		{
			m_dirty = true;
			continue;
		}

		Worker::Status status = info.worker->FrameUpdate( deltaTime );
		if( status != Worker::Continue )
		{
			info.Kill();
			m_dirty = true;
		}
	}

	return Worker::Continue;
}

///////////////////////////////////////////////////////////////////////////////
Worker::Status MassWorker::HandleEvent( const SDL_Event& event )
{
	std::list< WorkerInfo >::iterator iter;

	for( iter = m_workers.begin(); iter != m_workers.end(); iter++ )
	{
		WorkerInfo& info = *iter;
		if( ! info.IsAlive() )
		{
			m_dirty = true;
			continue;
		}

		Worker::Status status = info.worker->HandleEvent( event );
		if( status != Worker::Continue )
		{
			info.Kill();
			m_dirty = true;
		}
	}

	return Worker::Continue;
}

///////////////////////////////////////////////////////////////////////////////

