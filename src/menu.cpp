#include "menu.h"
#include "log.h"
#include "assert.h"
#include <string>

////////////////////////////////////////////////////////////////////////////////////
Menu::Menu( void )
	: m_parent( NULL )
	, m_child( NULL )
	, m_valid( true )
	, m_items()
	, m_current(0)
{
	// nothing
}

////////////////////////////////////////////////////////////////////////////////////
Menu::~Menu( void )
{
	// ignore m_parent

	if( m_child )
		delete m_child;

	for( size_t i = 0; i < m_items.size(); ++i )
		delete m_items[i];
}

////////////////////////////////////////////////////////////////////////////////////
void Menu::Push( Menu* child )
{
	ASSERT( child );
	ASSERT( child->valid() );
	ASSERT( ! m_child );

	if( m_child )
		delete m_child;

	m_child = child;
	child->m_parent = this;
}

////////////////////////////////////////////////////////////////////////////////////
void Menu::Add( Menu::Entry* entry )
{
	ASSERT( entry );
	m_items.push_back( entry );
}

////////////////////////////////////////////////////////////////////////////////////
void Menu::Handle( const SDL_Event& event )
{
	if( m_child )
	{
		m_child->Handle( event );

		if( ! m_child->valid() )
		{
			delete m_child;
			m_child = NULL;
		}
		return;
	}

	bool handled = true;
	switch( event.type )
	{
		case SDL_KEYDOWN:
			switch( event.key.keysym.sym )
			{
				case SDLK_UP:
					if( m_current > 0 )
						--m_current;
					break;

				case SDLK_DOWN:
					if( (m_current + 1) < m_items.size() )
						++m_current;
					break;

				case SDLK_ESCAPE:
					Exit();
					break;

				default:
					handled = false;
					break;
			}
			break;

		default:
			handled = false;
			break;
	}

	if( ! handled )
		m_items[ m_current ]->Handle( *this, event );
}

////////////////////////////////////////////////////////////////////////////////////
void Menu::Compile( std::vector< std::string >& items, unsigned& current )
{
	if( m_child )
	{
		m_child->Compile( items, current );
	}
	else
	{
		current = m_current;
		for( size_t i = 0; i < m_items.size(); ++i )
			items.push_back( m_items[i]->name() );
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
SimpleEntry::SimpleEntry( const std::string& name )
	: Menu::Entry()
	, m_name( name )
{
	// nothing
}

SimpleEntry::~SimpleEntry( void )
{
	// nothing
}

void SimpleEntry::Handle( Menu& menu, const SDL_Event& event )
{
	switch( event.type )
	{
		case SDL_KEYDOWN:
			switch( event.key.keysym.sym )
			{
				case SDLK_RETURN:
				case SDLK_KP_ENTER:
					Execute( menu );
					break;

				default:
					break;
			}
		default:
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

TestEntry::TestEntry( const std::string& name )
	: SimpleEntry( name )
{} // nothing

TestEntry::~TestEntry( void )
{} // nothing

void TestEntry::Execute( Menu& menu )
{
	logInfo( "Menu entry '%s' activated.", name().c_str() );
}



