#include "worker.h"
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
Worker::Status MassWorker::Update( float deltaTime )
{
	std::list< WorkerInfo >::iterator iter;

	for( iter = m_workers.begin(); iter != m_workers.end(); iter++ )
	{
		WorkerInfo& info = *iter;

		if( info.IsAlive() )
		{
			if( Worker::Continue != info.worker->Update( deltaTime ) )
			{
				info.Kill();
				m_dirty = true;
			}
		}
		else if( ! m_dirty )
		{
			m_dirty = true;
		}
	}

	return Worker::Continue;
}

///////////////////////////////////////////////////////////////////////////////
Worker::Status MassWorker::Handle( const SDL_Event& event )
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

		Worker::Status status = info.worker->Handle( event );
		if( status != Worker::Continue )
		{
			info.Kill();
			m_dirty = true;
		}
	}

	return Worker::Continue;
}

///////////////////////////////////////////////////////////////////////////////
void MassWorker::Render( void )
{
	std::list< WorkerInfo >::iterator iter;
	for( iter = m_workers.begin(); iter != m_workers.end(); iter++ )
	{
		WorkerInfo& info = *iter;
		if( info.IsAlive() )
			info.worker->Render();
	}
}

///////////////////////////////////////////////////////////////////////////////
void MassWorker::Pause( void )
{
	std::list< WorkerInfo >::iterator iter;
	for( iter = m_workers.begin(); iter != m_workers.end(); iter++ )
	{
		WorkerInfo& info = *iter;
		if( info.IsAlive() )
			info.worker->Pause();
		else
			m_dirty = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
void MassWorker::Resume( void )
{
	std::list< WorkerInfo >::iterator iter;
	for( iter = m_workers.begin(); iter != m_workers.end(); iter++ )
	{
		WorkerInfo& info = *iter;
		if( info.IsAlive() )
			info.worker->Resume();
		else
			m_dirty = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
