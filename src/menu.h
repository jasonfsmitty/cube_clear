#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include "sdl.h"

class Menu
{
	public:
		struct Entry
		{
			virtual std::string name( void ) = 0;
			virtual void Handle( Menu& menu, const SDL_Event& event ) = 0;
		};

		Menu( void );
		~Menu( void );

		void Add( Entry* entry );
		void Push( Menu* child );
		void Handle( const SDL_Event& event );

		void Compile( std::vector< std::string >& items, unsigned& current );

		// leave this menu, returning to the parent (if applicable)
		void Exit( void ) { m_valid = false; }

		bool valid( void ) const { return m_valid; }
		Menu* parent( void ) { return m_parent; }
		Menu* child( void ) { return m_child; }

	private:

		Menu* m_parent;
		Menu* m_child;

		bool m_valid;
		std::vector< Entry* > m_items;
		unsigned m_current; // index
};


class SimpleEntry : public Menu::Entry
{
	public:
		SimpleEntry( const std::string& name );
		virtual ~SimpleEntry( void );

		virtual void Handle( Menu& menu, const SDL_Event& event );
		virtual std::string name( void ) { return m_name; }

	protected:
		virtual void Execute( Menu& menu ) = 0;

		const std::string m_name;
};

class TestEntry : public SimpleEntry
{
	public:
		TestEntry( const std::string& name );
		virtual ~TestEntry( void );

	protected:
		virtual void Execute( Menu& menu );
};

class ExitEntry : public SimpleEntry
{
	public:
		ExitEntry( const std::string& name ) : SimpleEntry( name ) {}
		virtual ~ExitEntry( void ) {}

	protected:
		virtual void Execute( Menu& menu )
		{ menu.Exit(); }
};

#endif /* MENU_H */

