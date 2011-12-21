// title screen implementation

#include "title.h"
#include <GL/gl.h>

///////////////////////////////////////////////////////////////////////////////////////
TitleScreen::TitleScreen( void )
	: Worker()
	, m_uptime( 0.0f )
	, m_alive( true )
	, m_state( TITLE )
	, m_cubes()
	, m_game( m_cubes )
	, m_font()
{
	GotoTitleState();
	m_font.Load( "verabd.ttf", 24 );

	// build the menu
	m_menu.Add( new TestEntry( "Start" ) );
	m_menu.Add( new TestEntry( "High Scores" ) );
	m_menu.Add( new TestEntry( "Options" ) );
	m_menu.Add( new TestEntry( "About" ) );
	m_menu.Add( new ExitEntry( "Exit" ) );
}

///////////////////////////////////////////////////////////////////////////////////////
TitleScreen::~TitleScreen( void )
{
	// nothing
}

///////////////////////////////////////////////////////////////////////////////////////
void TitleScreen::GotoTitleState( void )
{
	m_state = TITLE;
	m_game.Reset();
	logDebug( "TitleScreen: TITLE state" );
}

///////////////////////////////////////////////////////////////////////////////////////
void TitleScreen::GotoGameState( void )
{
	m_state = GAME;
	m_game.Resume();
	logDebug( "TitleScreen: GAME state" );
}

///////////////////////////////////////////////////////////////////////////////////////
void TitleScreen::GotoMenuState( void )
{
	m_state = MENU;
	logDebug( "TitleScreen: TITLE state" );
}

///////////////////////////////////////////////////////////////////////////////////////
Worker::Status TitleScreen::Update( float deltaTime )
{
	m_uptime += deltaTime;
	switch( m_state )
	{
		case TITLE:
			m_cubes.Update( deltaTime );
			break;

		case GAME:
			if( Worker::Continue != m_game.Update( deltaTime ) )
				GotoTitleState();
			break;

		case MENU:
			m_cubes.Update( deltaTime );
			break;
	}
	return Worker::Continue;
}

///////////////////////////////////////////////////////////////////////////////////////
void TitleScreen::Render( void )
{
	switch( m_state )
	{
		case TITLE:
			RenderTitle();
			break;

		case MENU:
			RenderMenu();
			break;

		case GAME:
			m_game.Render();
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
void TitleScreen::RenderTitle( void )
{
	m_cubes.Render();

	GlLayout layout;
	GlPrinter printer( m_font );
	glColor4f( 1.0f, 1.0f, 1.0f, 0.60f );
	printer.Print( 10, 10, "Cube Clear v0.1" );
}

///////////////////////////////////////////////////////////////////////////////////////
void TitleScreen::RenderMenu( void )
{
	m_cubes.Render();

	GlLayout layout;
	GlPrinter printer( m_font );
	glColor4f( 1.0f, 1.0f, 1.0f, 0.60f );
	printer.Print( 10, 10, "Cube Clear v0.1" );

	// Render the menu

	std::vector< std::string > items;
	unsigned current;

	m_menu.Compile( items, current );

	const float screenWidth  = (float) layout.width();
	const float screenHeight = (float) layout.height();
	const float fontHeight   = m_font.height();
	const float alphaFactor  = 0.3f;

	for( unsigned i = 0; i < items.size(); ++i )
	{
		unsigned tmp = m_font.width( items[i] );
		float offset = float( int(current) - int(i) );
		float x = ( screenWidth - float( tmp ) ) * 0.5f;
		float y = ( screenHeight - fontHeight ) * 0.5f + ( offset * fontHeight );

		float alpha = (offset < 0.0f) ? (1.0f + offset * alphaFactor) : (1.0f - offset * alphaFactor);
		glColor4f( 1.0f, 1.0f, 1.0f, alpha );
		printer.Print( x, y, items[i] );
	}

}


///////////////////////////////////////////////////////////////////////////////////////
Worker::Status TitleScreen::HandleTitleEvent( const SDL_Event& event )
{
	switch( event.type )
	{
		case SDL_KEYDOWN:
			switch( event.key.keysym.sym )
			{
				case SDLK_RETURN:
				case SDLK_KP_ENTER:
					GotoMenuState();
					return Worker::Continue;

				case SDLK_ESCAPE:
					m_alive = false;
					return Worker::Exit;

				default:
					break;
			}
			break;

		default:
			break;
	}
	return Worker::Continue;
}

///////////////////////////////////////////////////////////////////////////////////////
Worker::Status TitleScreen::HandleMenuEvent( const SDL_Event& event )
{
	m_menu.Handle( event );
	return m_menu.valid() ? Worker::Continue : Worker::Exit;
}

///////////////////////////////////////////////////////////////////////////////////////
Worker::Status TitleScreen::HandleGameEvent( const SDL_Event& event )
{
	if( Worker::Continue != m_game.Handle( event ) )
		GotoTitleState();

	return Worker::Continue;
}

///////////////////////////////////////////////////////////////////////////////////////
Worker::Status TitleScreen::Handle( const SDL_Event& event )
{
	switch( event.type )
	{
		case SDL_QUIT:
			m_alive = false;
			return Worker::Exit;
		default:
			break;
	}

	switch( m_state )
	{
		case TITLE:  return HandleTitleEvent( event );
		case MENU:   return HandleMenuEvent( event );
		case GAME:   return HandleGameEvent( event );
	}

	return Worker::Exit;
}

///////////////////////////////////////////////////////////////////////////////////////
void TitleScreen::Pause( void )
{
	switch( m_state )
	{
		case TITLE:
		case MENU:
			m_cubes.Pause();
			break;
		case GAME:
			m_game.Pause();
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////
void TitleScreen::Resume( void )
{
	switch( m_state )
	{
		case TITLE:
		case MENU:
			m_cubes.Resume();
			break;
		case GAME:
			m_game.Resume();
			break;
	}
}

