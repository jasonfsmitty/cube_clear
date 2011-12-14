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
		int height( void ) const { return m_height; };
		int list_base( void ) const { return m_base; };

	private:

		int m_height;
		GLuint m_base;
		GLuint m_textures[ 128 ];
		GLuint m_widths[ 128 ];
};


class GlPrinter
{
	public:
		GlPrinter( GlFont& font );
		~GlPrinter( void );

		void Print( int x, int y, const std::string& text );

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
		GlLayout( GlFont& font, int width, int height );
		~GlLayout( void );
};


#endif /* FONT_H */

