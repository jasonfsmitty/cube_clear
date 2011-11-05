#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include "worker.h"
#include <vector>

struct Cursor 
{
	int x;
	int y;

	Cursor( int _x, int _y ) : x(_x), y(_y) {}
	Cursor( const Cursor& p ) : x(p.x), y(p.y) {}

	Cursor& operator=( const Cursor& p )
		{ x=p.x; y=p.y; return *this; }

	bool operator==( const Cursor& p )
		{ return (x==p.x) && (y==p.y); }
};

struct Gem
{
	enum State
	{
		IDLE,
		SWAPPING,
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

	void Move( int x, int y )
	{
		state = SWAPPING;
		dx = float(x);
		dy = float(y);
		clear = 0.0f;
	}

	void Drop( int y )
	{
		state = FALLING;
		dx = 0.0f;
		dy = float(y);
		clear = 0.0f;
	}

	void Clear( void )
	{
		state = CLEARING;
		dx = dy = 0.0f;
		clear = 1.0f;
	}

	void Idle( void )
	{
		state = IDLE;
		dx = dy = clear = 0.0f;
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

		virtual Status Update( float deltaTime );
		virtual Status Handle( const SDL_Event& event );
		virtual void Render( void );

		virtual bool IsAlive( void ) { return m_alive; }

		virtual void Pause( void );
		virtual void Resume( void );

		Gem* gem( int x, int y );
		void set_gem( int x, int y, Gem* gem );

		int size( void ) const { return m_size; }
		int num_types( void ) const { return m_numTypes; }

		// input
		void HandleEnter( void );
		void HandleUp( void );
		void HandleDown( void );
		void HandleLeft( void );
		void HandleRight( void );

	private:

		void Swap( int x1, int y1, int x2, int y2 );

		const int m_size;
		const int m_numTypes;

		State m_state;
		std::vector< Gem* > m_gems;
		Cursor m_cursor;

		bool m_alive;
};

#endif /* GAME_BOARD_H */
