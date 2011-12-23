#ifndef FONT_H
#define FONT_H

#include <string>
#include <GL/gl.h>

class GlFont
{
	public:
		GlFont( void );
		~GlFont( void );

		bool Load( const std::string& fontname, int height );
		void Release( void );

		void Print( int x, int y, const std::string& text );
		void Print( float x, float y, const std::string& text );

		// These are all that's needed for writing text
		float height( void ) const { return m_height; };
		float width( const std::string& text );
		float width( char c ) { return (c>0 && c<128) ? m_widths[(int)c] : 0.0f; }

	private:

		float m_height;
		GLuint m_base;
		GLuint m_textures[ 128 ];
		float m_widths[ 128 ];
};


class GlPrinter
{
	public:
		enum Alignment
		{
			Left,
			Right,
			Center,
		};

		GlPrinter( GlFont& font );
		~GlPrinter( void );

		void Print( const std::string& text );

		void Print( float x, float y, const std::string& text )
		{
			set_pos( x, y );
			Print( text );
		}

		GlFont& font( void ) { return m_font; }

		float x( void ) const { return m_xpos; }
		float y( void ) const { return m_ypos; }
		void set_pos( float x, float y ) { m_xpos = x; m_ypos = y; }

		float width( void ) const { return m_width; }
		void set_width( float w ) { m_width = w; }

		Alignment align( void ) const { return m_alignment; }
		void set_align( Alignment align ) { m_alignment = align; }

	private:

		// not implemented
		GlPrinter( void );
		GlPrinter( const GlPrinter& );
		GlPrinter& operator=( const GlPrinter& );

		GlFont& m_font;
		Alignment m_alignment;
		float m_width;
		float m_xpos;
		float m_ypos;
};


class GlLayout
{
	public:
		GlLayout( int width, int height );
		GlLayout( void );
		~GlLayout( void );

		int height( void ) const { return m_height; };
		int width( void ) const { return m_width; };

	private:
		int m_width;
		int m_height;
};


#endif /* FONT_H */

