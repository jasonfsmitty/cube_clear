#include "board.h"
#include "assert.h"
#include "event.h"
#include <GL/gl.h>

namespace {
	const float FIELD_MARGIN = 0.25f;
	const float FIELD_SCALE  = 2.0f;

	bool IsValid( int size, int i )
		{ return (i >= 0) && (i < size); }

	bool IsValid( int size, const Point& p )
		{ return IsValid( size, p.x ) && IsValid( size, p.y ); }

	int ToIndex( int size, int x, int y )
		{ return ( y * ( size ) ) + x; }

	int ToIndex( int size, const Point& p )
		{ return ( p.y * ( size ) ) + p.x; }

	Point FromIndex( int size, int i )
		{ return Point( i % size, i / size ); }

	Gem* NewGem( const int numTypes )
	{
		Gem* gem = new Gem();
		gem->type = rand() % (numTypes);
		return gem;
	}

	void DeleteGem( Gem* gem )
	{
		if( gem )
			delete gem;
	}
	void DrawSquare( float size, float x, float y )
	{
		glPushMatrix();
		glTranslatef( x, y, 0.0f );
		glBegin( GL_TRIANGLE_STRIP );
			glVertex3f(  size,  size, -size );
			glVertex3f( -size,  size, -size );
			glVertex3f(  size, -size, -size );
			glVertex3f( -size, -size, -size );
		glEnd();
		glPopMatrix();
	}

	void DrawGem( float x, float y )
	{
		static const float size = 0.80f;
		static const float margin = (1.0f - size) / 2.0f;
		static const float width = 1.0f - margin;

		glPushMatrix();
		glTranslatef( x, y, 0.0f );
		glBegin( GL_TRIANGLE_STRIP );
			glTexCoord2i( 0, 0 );  glVertex2f( margin, margin );
			glTexCoord2i( 1, 0 );  glVertex2f( width,  margin );
			glTexCoord2i( 0, 1 );  glVertex2f( margin, width );
			glTexCoord2i( 1, 1 );  glVertex2f( width,  width );
		glEnd();
		glPopMatrix();
	}

	void DrawFieldBackground( int size )
	{
		const float left   = 0.0f;
		const float right  = float(size);
		const float bottom = left;
		const float top    = right;

		// draw a background square to set the field out from the background
		glBindTexture( GL_TEXTURE_2D, 0 );
		glColor4f( 0.7f, 0.7f, 0.7f, 0.55f );
		glBegin( GL_TRIANGLE_STRIP );
			glVertex3f( left,  top,    0.0f );
			glVertex3f( right, top,    0.0f );
			glVertex3f( left,  bottom, 0.0f );
			glVertex3f( right, bottom, 0.0f );
		glEnd();

		// draw the grid
		glLineWidth( 2.0f );
		glColor4f( 0.8f, 0.8f, 0.8f, 0.75f );
		glBegin( GL_LINES );
		for( int i = 0; i <= size; i++ )
		{
			glVertex2i( i, 0 );
			glVertex2i( i, size );
			glVertex2i( 0, i );
			glVertex2i( size, i );
		}
		glEnd();
	}

	void DrawField( const std::vector< Gem* > gems, const int size )
	{
		float colors[][4] =
		{
			{ 1.0f, 0.0f, 0.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f, 1.0f },
			{ 0.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f },
		};

		for( unsigned i=0; i < gems.size(); i++ )
		{
			Gem* gem = gems[i];
			if( ! gem )
				continue;

			Point point = FromIndex( size, i );

			float x( point.x );
			float y( point.y );
			float alpha = 1.0f;

			switch( gem->state )
			{
				case Gem::IDLE:
					// no change;
					break;

				case Gem::CLEARING:
					alpha = gem->clear;
					break;

				case Gem::SWAP_HORIZ:
				case Gem::SWAP_VERT:
					x += gem->dx;
					y += gem->dy;
					break;

				case Gem::FALLING:
					y += gem->dy;
					break;
			}

			colors[ gem->type ][3] = alpha;
			glColor4fv( colors[ gem->type ] );

			// Finally draw the gem ...
			DrawGem( x, y );
		}
	}

	void DrawCursor( bool pressed, const Point& point )
	{
		const float length( pressed ? 1.0f : 0.25f );
		const float alpha ( pressed ? 1.0f : 0.50f );

		glPushMatrix();
		glTranslatef( float(point.x), float(point.y), 0.0f );

		glBindTexture( GL_TEXTURE_2D, 0 );
		glLineWidth( 5.0 );
		glColor4f( 1.0f, 1.0f, 1.0f, alpha );

		glBegin( GL_LINES );
			// lower left
			glVertex2f( 0.0f, 0.0f );
			glVertex2f( 0.0f, length );
			glVertex2f( 0.0f, 0.0f );
			glVertex2f( length, 0.0f );

			// top right
			glVertex2f( 0.0f, 1.0f );
			glVertex2f( length, 1.0f );
			glVertex2f( 0.0f, 1.0f );
			glVertex2f( 0.0f, 1.0f - length );

			// bottom right
			glVertex2f( 1.0f, 0.0f );
			glVertex2f( 1.0f, length );
			glVertex2f( 1.0f, 0.0f );
			glVertex2f( 1.0f - length, 0.0f );

			// top right
			glVertex2f( 1.0f, 1.0f );
			glVertex2f( 1.0f, 1.0f - length );
			glVertex2f( 1.0f, 1.0f );
			glVertex2f( 1.0f - length, 1.0f );
		glEnd();

		glPopMatrix();
	}
}

////////////////////////////////////////////////////////////////////////////////
Board::Board( int size, int numTypes )
	: m_size( size )
	, m_numTypes( numTypes )
	, m_state( IDLE )
	, m_gems( (size)*(size), NULL )
	, m_cursor( size/2, size/2 )
	, m_alive( true )
{
	// nothing
}

////////////////////////////////////////////////////////////////////////////////
Board::~Board( void )
{
	for( unsigned i = 0; i < m_gems.size(); i++ )
		DeleteGem( m_gems[i] );
}

////////////////////////////////////////////////////////////////////////////////
void Board::Reset( void )
{
	logInfo( "Resetting game board" );

	for( unsigned i=0; i < m_gems.size(); i++ )
	{
		DeleteGem( m_gems[i] );
		m_gems[i] = NewGem( m_numTypes );
	}

	m_cursor.x = m_size/2;
	m_cursor.y = m_size/2;

	GotoSwappingState(); // this will force an initial check for matches
}

////////////////////////////////////////////////////////////////////////////////
void Board::GotoIdleState( void )
{
	m_state = IDLE;
	logDebug( "board.state == IDLE" );
}

////////////////////////////////////////////////////////////////////////////////
void Board::GotoSelectedState( void )
{
	m_state = SELECTED;
	logDebug( "board.state == SELECTED with cursor=%s", m_cursor.str().c_str() );
}

////////////////////////////////////////////////////////////////////////////////
void Board::GotoSwappingState( void )
{
	m_state = SWAPPING;
	logDebug( "board.state == SWAPPING" );
}

////////////////////////////////////////////////////////////////////////////////
void Board::GotoClearingState( void )
{
	m_state = CLEARING;
	logDebug( "board.state == CLEARING" );
}

////////////////////////////////////////////////////////////////////////////////
void Board::GotoFallingState( void )
{
	m_state = FALLING;
	logDebug( "board.state == FALLING" );
}

////////////////////////////////////////////////////////////////////////////////
bool Board::CheckForOneWayMatches( bool flipped )
{
	const int numForClear = 3;
	bool foundClearing = false;

	// check each row
	for( int row = 0; row < m_size; row++ )
	{
		int type = -1;
		int count = 0;

		for( int col = 0; col < m_size; ++col )
		{
			Gem* g = get_gem( FlipPoint( col, row, flipped ) );

			if( ! g )
			{
				// no block, reset
				type = -1;
				count = 0;
			}
			else if( g->type != type )
			{
				type = g->type;
				count = 1;
			}
			else
			{
				if( ++count == numForClear )
				{
					foundClearing = true;
					for( int i=(col-count+1); i <= col; i++ )
						get_gem( FlipPoint( i, row, flipped ) )->Clear();
				}
				else if( count > numForClear )
				{
					g->Clear();
				}
				else
				{
					// not enough for clear, do nothing
				}
			}
		}
	}

	return foundClearing;
}

////////////////////////////////////////////////////////////////////////////////
bool Board::CheckForMatches( void )
{
	bool horizontal = CheckForOneWayMatches( false );
	bool vertical   = CheckForOneWayMatches( true );
	return horizontal || vertical;
}

////////////////////////////////////////////////////////////////////////////////
void Board::MarkFalling( void )
{
	bool falling = false;

	Point p;
	for( p.x = 0; p.x < m_size; p.x++ )
	{
		int shift = 0;
		for( p.y = 0; p.y < m_size; p.y++ )
		{
			Gem* gem = get_gem( p );

			if( ! gem )
				shift++;
			else if( shift )
			{
				set_gem( p, NULL );
				set_gem( p.dy( -shift ), gem );
				gem->Drop( shift );
				falling = true;
			}
		}
	}

	if( falling )
		GotoFallingState();
	else
		GotoIdleState();
}

////////////////////////////////////////////////////////////////////////////////
bool Board::FillCleared( void )
{
	bool inserted = false;
	Point p;

	for( p.x = 0; p.x < m_size; p.x++ )
	{
		int start = -1;
		for( p.y = 0; p.y < m_size; p.y++ )
		{
			Gem* gem = get_gem( p );
			if( gem )
				continue;

			if( start < 0 )
				start = p.y;

			gem = NewGem( m_numTypes );

			gem->Drop( m_size - start );
			set_gem( p, gem );
			inserted = true;
		}
	}

	return inserted;
}

////////////////////////////////////////////////////////////////////////////////
Worker::Status Board::Update( float deltaTime )
{
	const float swapRate = 4.0f;
	const float fallRate = 4.0f;
	const float clearRate = 4.0f;

	bool clearing = false;
	bool swapping = false;
	bool falling  = false;

	for( unsigned i = 0; i < m_gems.size(); i++ )
	{
		Gem* gem = m_gems[i];
		if( ! gem )
			continue;

		switch( gem->state )
		{
			case Gem::IDLE:
				// nothing
				break;

			case Gem::SWAP_HORIZ:
				if( gem->dx > 0.0f )
				{
					gem->dx -= (deltaTime * swapRate);
					if( gem->dx <= 0.0f )
						gem->Idle();
					else
						swapping = true;
				}
				else if( gem->dx < 0.0f )
				{
					gem->dx += (deltaTime * swapRate);
					if( gem->dx >= 0.0f )
						gem->Idle();
					else
						swapping = true;
				}
				else
				{
					logError( "gem[%p]: invalid dx value for swapping horiz, marking idle", gem );
					gem->Idle();
				}
				break;

			case Gem::SWAP_VERT:
				if( gem->dy > 0.0f )
				{
					gem->dy -= (deltaTime * swapRate);
					if( gem->dy <= 0.0f )
						gem->Idle();
					else
						swapping = true;
				}
				else if( gem->dy < 0.0f )
				{
					gem->dy += (deltaTime * swapRate);
					if( gem->dy >= 0.0f )
						gem->Idle();
					else
						swapping = true;
				}
				else
				{
					logError( "gem[%p]: invalid dy value for swapping vert, marking idle", gem );
					gem->Idle();
				}
				break;

			case Gem::CLEARING:
				gem->clear -= (deltaTime * clearRate);
				if( gem->clear <= 0.0f )
				{
					m_gems[i] = NULL;
					DeleteGem( gem );
				}
				else
					clearing = true;
				break;

			case Gem::FALLING:
				gem->dy -= ( deltaTime * fallRate );
				if( gem->dy <= 0.0f )
					gem->Idle();
				else
					falling = true;
				break;
		}
	}

	switch( m_state )
	{
		case IDLE:
		case SELECTED:
			// nothing
			break;

		case SWAPPING:
			if( ! swapping )
			{
				if( CheckForMatches() )
					GotoClearingState();
				else
					GotoIdleState();
			}
			break;

		case CLEARING:
			if( ! clearing )
			{
				MarkFalling();
				FillCleared();
				GotoFallingState();
			}
			break;

		case FALLING:
			if( ! falling )
			{
				if( CheckForMatches() )
					GotoClearingState();
				else
					GotoIdleState();
			}
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
					if( IsValid( m_size, m_cursor.dy(1) ) )
					{
						++(m_cursor.y);
						logDebug( "move cursor up: %s", m_cursor.str().c_str() );
					}
					break;

				case event::Board::DOWN:
					if( IsValid( m_size, m_cursor.dy(-1) ) )
					{
						--(m_cursor.y);
						logDebug( "move cursor down: %s", m_cursor.str().c_str() );
					}
					break;

				case event::Board::RIGHT:
					if( IsValid( m_size, m_cursor.dx(1) ) )
					{
						++(m_cursor.x);
						logDebug( "move cursor right: %s", m_cursor.str().c_str() );
					}
					break;

				case event::Board::LEFT:
					if( IsValid( m_size, m_cursor.dx(-1) ) )
					{
						--(m_cursor.x);
						logDebug( "move cursor left: %s", m_cursor.str().c_str() );
					}
					break;

				case event::Board::ENTER:
					GotoSelectedState();
					break;
			}
			break;

		case SELECTED:
			switch( board->input )
			{
				case event::Board::UP:
					Swap( m_cursor, m_cursor.dy(1) );
					break;

				case event::Board::DOWN:
					Swap( m_cursor, m_cursor.dy(-1) );
					break;

				case event::Board::LEFT:
					Swap( m_cursor, m_cursor.dx(-1) );
					break;

				case event::Board::RIGHT:
					Swap( m_cursor, m_cursor.dx(1) );
					break;

				case event::Board::ENTER:
					GotoIdleState();
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
void Board::Swap( const Point& p1, const Point& p2 )
{
	Gem* left = get_gem( p1 );
	Gem* right = get_gem( p2 );

	if( !left || !right )
		return;

	logDebug( "Swapping gems: p1=%s p2=%s", p1.str().c_str(), p2.str().c_str() );

	set_gem( p1, right );
	set_gem( p2, left );

	left->Swap( p1 - p2 );
	right->Swap( p2 - p1 );

	GotoSwappingState();
}

////////////////////////////////////////////////////////////////////////////////
void Board::Render( void )
{
	const float blockScale = 1.0f / float(m_size);

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	// glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glOrtho
		( 0.0f, 4.0f // left, right
		, 0.0f, 3.0f // bottom, top
		, 1.0f, -1.0f // near, far
		);

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glPushMatrix();

		glTranslatef( FIELD_MARGIN, FIELD_MARGIN, 0.0f );
		glScalef( FIELD_SCALE, FIELD_SCALE, 0.0f );

		glScalef( blockScale, blockScale, 0.0f );

		DrawFieldBackground( m_size );
		DrawField( m_gems, m_size );

		if( m_state == IDLE )
			DrawCursor( false, m_cursor );
		else if( m_state == SELECTED )
			DrawCursor( true, m_cursor );

	glPopMatrix();

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
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
Gem* Board::get_gem( const Point& p )
{
	if( ! IsValid( m_size, p ) )
		return NULL;
	return m_gems[ ToIndex( m_size, p ) ];
}

////////////////////////////////////////////////////////////////////////////////
void Board::set_gem( const Point& p, Gem* gem )
{
	ASSERT( IsValid( m_size, p.x ) );
	ASSERT( IsValid( m_size, p.y ) );

	m_gems[ ToIndex( m_size, p ) ] = gem;
}

