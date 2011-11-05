#include "board.h"
#include "assert.h"
#include "event.h"

namespace {

	bool IsValid( int size, int i )
		{ return (i > 0) && (i <= size); }

	int ToIndex( int size, int x, int y )
		{ return ( y * ( size + 1 ) ) + x; }

	int ToIndex( int size, const Cursor& p )
		{ return ( p.y * ( size + 1 ) ) + p.x; }

	Cursor FromIndex( int size, int i )
		{ return Cursor( (i % (size+1)), i / (size+1) ); }

}

////////////////////////////////////////////////////////////////////////////////
Board::Board( int size, int numTypes )
	: m_size( size )
	, m_numTypes( numTypes )
	, m_state( IDLE )
	, m_gems( (size+1)*(size+1), NULL )
	, m_cursor( size/2, size/2 )
	, m_alive( true )
{
	// nothing
}

////////////////////////////////////////////////////////////////////////////////
Board::~Board( void )
{
	for( unsigned i = 0; i < m_gems.size(); i++ )
	{
		if( m_gems[i] )
			delete m_gems[i];
	}
}

////////////////////////////////////////////////////////////////////////////////
Worker::Status Board::Update( float deltaTime )
{
	switch( m_state )
	{
		case IDLE:
			// nothing
			break;

		case SELECTED:
			// nothing
			break;

		case SWAPPING:
			// TODO
			break;

		case CLEARING:
			// TODO
			break;

		case FALLING:
			// TODO
			break;
	}

	return Worker::Continue;
}

////////////////////////////////////////////////////////////////////////////////
Worker::Status Board::Handle( const SDL_Event& event )
{
	if( ( event.type != SDL_USEREVENT ) || ( event.user.code != event::EVT_BOARD ) )
		return Worker::Continue;

	const event::Board* board = static_cast< const event::Board* >( event.user.data1 );
	ASSERT( board );

	switch( m_state )
	{
		case IDLE:
			switch( board->input )
			{
				case event::Board::UP:
					if( m_cursor.y < m_size )
					{
						++(m_cursor.y);
						logDebug( "move cursor up" );
					}
					break;

				case event::Board::DOWN:
					if( m_cursor.y > 1 )
					{
						--(m_cursor.y);
						logDebug( "move cursor down" );
					}
					break;

				case event::Board::RIGHT:
					if( m_cursor.x < m_size )
					{
						++(m_cursor.x);
						logDebug( "move cursor right" );
					}
					break;

				case event::Board::LEFT:
					if( m_cursor.x > 1 )
					{
						--(m_cursor.x);
						logDebug( "move cursor left" );
					}
					break;

				case event::Board::ENTER:
					m_state = SELECTED;
					logDebug( "cursor SELECT" );
					break;
			}
			break;

		case SELECTED:
			switch( board->input )
			{
				case event::Board::UP:
					Swap( m_cursor.x, m_cursor.y, m_cursor.x, m_cursor.y+1 );
					break;

				case event::Board::DOWN:
					Swap( m_cursor.x, m_cursor.y, m_cursor.x, m_cursor.y-1 );
					break;

				case event::Board::LEFT:
					Swap( m_cursor.x, m_cursor.y, m_cursor.x-1, m_cursor.y );
					break;

				case event::Board::RIGHT:
					Swap( m_cursor.x, m_cursor.y, m_cursor.x+1, m_cursor.y );
					break;

				case event::Board::ENTER:
					m_state = IDLE;
					break;
			}
			break;

		case SWAPPING:
			// no input allowed
			break;

		case CLEARING:
			// no input allowed
			break;

		case FALLING:
			// no input allowed
			break;
	}

	return Worker::Continue;
}

////////////////////////////////////////////////////////////////////////////////
void Board::Swap( int x1, int y1, int x2, int y2 )
{
	Gem* left = gem( x1, y1 );
	Gem* right = gem( x2, y2 );

	if( !left || !right )
		return;

	m_state = SWAPPING;

	set_gem( x1, y1, right );
	set_gem( x2, y2, left );
	left->Move( x1 - x2, y1 - y2 );
	right->Move( x2 - x1, y2 - y1 );
}

////////////////////////////////////////////////////////////////////////////////
void Board::Render( void )
{
	// TODO
}

////////////////////////////////////////////////////////////////////////////////
void Board::Pause( void )
{
	// nothing
}

////////////////////////////////////////////////////////////////////////////////
void Board::Resume( void )
{
	// nothing
}

////////////////////////////////////////////////////////////////////////////////
Gem* Board::gem( int x, int y )
{
	if( x < 1 || x > m_size ) return NULL;
	if( y < 1 || y > m_size ) return NULL;

	return m_gems[ y * (m_size+1) + x ];
}

////////////////////////////////////////////////////////////////////////////////
void Board::set_gem( int x, int y, Gem* gem )
{
	ASSERT( x > 0 && x <= m_size );
	ASSERT( y > 0 && y <= m_size );

	m_gems[ y * (m_size+1) + x ] = gem;
}

