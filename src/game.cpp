#include "game.h"
#include "event.h"

namespace {

	const unsigned GAME_SIZE = 7;
	const unsigned NUM_GEM_TYPES = 5;

	void SendBoardEvent( Worker& worker, event::Board::Input input )
	{
		event::Board board;
		board.input = input;

		SDL_Event event;
		event.type = SDL_USEREVENT;
		event.user.code = event::EVT_BOARD;
		event.user.data1 = &( board );

		worker.Handle( event );
	}

	void SendEnter( Worker& worker )
		{ SendBoardEvent( worker, event::Board::ENTER ); }

	void SendUp( Worker& worker )
		{ SendBoardEvent( worker, event::Board::UP ); }

	void SendDown( Worker& worker )
		{ SendBoardEvent( worker, event::Board::DOWN ); }

	void SendLeft( Worker& worker )
		{ SendBoardEvent( worker, event::Board::LEFT ); }

	void SendRight( Worker& worker )
		{ SendBoardEvent( worker, event::Board::RIGHT ); }
};

////////////////////////////////////////////////////////////////////////////////
Game::Game( Worker& background )
	: m_background( background )
	, m_board( GAME_SIZE, NUM_GEM_TYPES )
	, m_alive( true )
{
	// nothing
}

////////////////////////////////////////////////////////////////////////////////
Game::~Game( void )
{
	// nothing
}

////////////////////////////////////////////////////////////////////////////////
Worker::Status Game::Update( float deltaTime )
{
	m_background.Update( deltaTime );
	m_board.Update( deltaTime );

	return Worker::Continue;
}

////////////////////////////////////////////////////////////////////////////////
Worker::Status Game::Handle( const SDL_Event& event )
{
	switch( event.type )
	{
		case SDL_QUIT:
			m_alive = false;
			return Worker::Exit;

		case SDL_KEYDOWN:
			switch( event.key.keysym.sym )
			{
				case ' ':
				case SDLK_KP_ENTER:
				case SDLK_RETURN:
					SendEnter( m_board );
					break;

				case SDLK_UP:    SendUp( m_board ); break;
				case SDLK_DOWN:  SendDown( m_board ); break;
				case SDLK_LEFT:  SendLeft( m_board ); break;
				case SDLK_RIGHT: SendRight( m_board ); break;

				default:
					break;
			}
			break;

		default:
			break;
	}

	return Worker::Continue;
}

////////////////////////////////////////////////////////////////////////////////
void Game::Render( void )
{
	m_background.Render();
	m_board.Render();
}

////////////////////////////////////////////////////////////////////////////////
void Game::Pause( void )
{
	m_background.Pause();
	m_board.Pause();
}

////////////////////////////////////////////////////////////////////////////////
void Game::Resume( void )
{
	m_background.Resume();
	m_board.Resume();
}

