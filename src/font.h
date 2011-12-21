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

		// These are all that's needed for writing text
		float height( void ) const { return m_height; };
		float width( const std::string& text );
		float width( char c ) { return (c>0 && c<128) ? m_widths[(int)c] : 0.0f; }

		int list_base( void ) const { return m_base; };

	private:

		float m_height;
		GLuint m_base;
		GLuint m_textures[ 128 ];
		float m_widths[ 128 ];
};


class GlPrinter
{
	public:
		GlPrinter( GlFont& font );
		~GlPrinter( void );

		void Print( int x, int y, const std::string& text );
		void Print( float x, float y, const std::string& text );

		GlFont& font( void ) { return m_font; }

	private:
		// not implemented
		GlPrinter( void );
		GlPrinter( const GlPrinter& );
		GlPrinter& operator=( const GlPrinter& );

		GlFont& m_font;
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

