#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include "worker.h"
#include <vector>
#include <sstream>
#include "log.h"
#include "score.h"

struct Point 
{
	int x;
	int y;

	Point( int _x, int _y ) : x(_x), y(_y) {}
	Point( const Point& p ) : x(p.x), y(p.y) {}
	Point( void ) : x(0), y(0) {}

	Point& operator=( const Point& p )
		{ x=p.x; y=p.y; return *this; }

	bool operator==( const Point& p ) const
		{ return (x==p.x) && (y==p.y); }

	std::string str( void ) const
	{
		std::ostringstream oss;
		oss << "(" << x << ", " << y << ")";
		return oss.str();
	}

	Point dx( int change ) const
		{ return Point( x+change, y ); }

	Point dy( int change ) const
		{ return Point( x, y+change ); }

	Point operator+( const Point& right ) const
		{ return Point( x + right.x, y + right.y ); }

	Point operator-( const Point& right ) const
		{ return Point( x - right.x, y - right.y ); }

};

inline Point FlipPoint( int x, int y, bool flipped )
	{ return Point( (flipped ? y : x), (flipped ? x : y) ); }

struct Gem
{
	enum State
	{
		IDLE,
		SWAP_HORIZ,
		SWAP_VERT,
		CLEARING,
		FALLING,
	};

	int type;
	State state;
	float dx;
	float dy;
	float clear;

	Gem( void ) : type(0)
		{ Idle(); }

	void Swap( const Point& dist )
	{
		const bool horiz = ( dist.x != 0 );
		const float value = float( horiz ? dist.x : dist.y );

		if( horiz )
		{
			state = SWAP_HORIZ;
			dx = value;
			dy = 0.0f;
		}
		else
		{
			state = SWAP_VERT;
			dx = 0.0f;
			dy = value;
		}
		logDebug( "gem[%p]: SWAPPING horiz=%i dx=%.2f dy=%.2f", this, horiz, dx, dy );
	}

	void Drop( int y )
	{
		state = FALLING;
		dx = 0.0f;
		dy = float(y);
		clear = 0.0f;
		logDebug( "gem[%p]: DROPPING dy=%.2f", this, dy );
	}

	void Clear( void )
	{
		state = CLEARING;
		dx = dy = 0.0f;
		clear = 1.0f;
		logDebug( "gem[%p]: CLEARING", this );
	}

	void Idle( void )
	{
		state = IDLE;
		dx = dy = clear = 0.0f;
		logDebug( "gem[%p]: IDLE", this );
	}
};

class Board : public Worker
{
	public:
		enum State
		{
			IDLE,
			SELECTED,
			SWAPPING,
			CLEARING,
			FALLING,
		};

		Board( int size, int numTypes );
		~Board( void );

		void Reset( void );

		virtual Status Update( float deltaTime );
		virtual Status Handle( const SDL_Event& event );
		virtual void Render( void );

		virtual bool IsAlive( void ) { return m_alive; }

		virtual void Pause( void );
		virtual void Resume( void );

		Gem* get_gem( const Point& p );
		void set_gem( const Point& p, Gem* gem );

		int size( void ) const { return m_size; }
		int num_types( void ) const { return m_numTypes; }

		// input
		void HandleEnter( void );
		void HandleUp( void );
		void HandleDown( void );
		void HandleLeft( void );
		void HandleRight( void );

	private:

		//void Swap( int x1, int y1, int x2, int y2 );
		bool Swap( const Point& p1, const Point& p2 );

		bool CheckForMatches( void );
		bool CheckForOneWayMatches( bool flipped );
		void MarkFalling( void );
		bool FillCleared( void );

		void GotoIdleState( void );
		void GotoSelectedState( void );
		void GotoSwappingState( void );
		void GotoClearingState( void );
		void GotoFallingState( void );

		const int m_size;
		const int m_numTypes;

		State m_state;
		std::vector< Gem* > m_gems;
		Point m_cursor;

		bool m_alive;
		Score m_score;
};

#endif /* GAME_BOARD_H */
