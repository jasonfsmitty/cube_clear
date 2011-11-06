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
		{ return Point( (i % (size)), i / (size) ); }

	Gem* NewGem( const int numTypes )
	{
		Gem* gem = new Gem();
		gem->type = rand() % (numTypes + 1);
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

				case Gem::SWAPPING:
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

	void DrawCursor( bool pressed, int x, int y )
	{
		const float length( pressed ? 1.0f : 0.25f );
		const float alpha ( pressed ? 1.0f : 0.50f );

		glPushMatrix();
		glTranslatef( float(x), float(y), 0.0f );

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

	m_state = IDLE;
	m_cursor.x = m_size/2;
	m_cursor.y = m_size/2;
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
					if( IsValid( m_size, m_cursor.dy(1) ) )
					{
						++(m_cursor.y);
						logDebug( "move cursor up" );
					}
					break;

				case event::Board::DOWN:
					if( IsValid( m_size, m_cursor.dy(-1) ) )
					{
						--(m_cursor.y);
						logDebug( "move cursor down" );
					}
					break;

				case event::Board::RIGHT:
					if( IsValid( m_size, m_cursor.dx(1) ) )
					{
						++(m_cursor.x);
						logDebug( "move cursor right" );
					}
					break;

				case event::Board::LEFT:
					if( IsValid( m_size, m_cursor.dx(-1) ) )
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
			DrawCursor( false, m_cursor.x, m_cursor.y );
		else if( m_state == SELECTED )
			DrawCursor( true, m_cursor.x, m_cursor.y );

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

