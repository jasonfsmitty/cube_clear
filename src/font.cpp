#include "font.h"
#include "assert.h"
#include "log.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

//#define MAX_TEXT_SIZE  512

namespace {

	///////////////////////////////////////////////////////////////////////////////////
	int NextPow2( int start )
	{
		int result = 1;
		while( result < start )
			result <<= 1;
		return result;
	}

	///////////////////////////////////////////////////////////////////////////////////
	bool _CreateCharacter
		( FT_Face& face
		, char c
		, GLuint listBase
		, GLuint& texture
		, GLuint& fontWidth
		)
	{
		if( FT_Load_Glyph( face, FT_Get_Char_Index( face, c ), FT_LOAD_DEFAULT ) )
		{
			logError( "Failed to load font face for char '%c'", c );
			return false;
		}

		FT_Glyph glyph;
		if( FT_Get_Glyph( face->glyph, &glyph ) )
		{
			logError( "FT_Get_Glyph failed for char '%c'", c );
			return false;
		}

		FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
		FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph) glyph;

		FT_Bitmap& bitmap = bitmapGlyph->bitmap;

		const int width = NextPow2( bitmap.width );
		const int height = NextPow2( bitmap.rows );

		GLubyte expandedData[ 2 * width * height ];

		for( int j=0; j<height; ++j )
		{
			for( int i=0; i<width; ++i )
			{
				expandedData[ 2 * (i + j * width) ] = 255;
				expandedData[ 2 * (i + j * width) + 1 ] =
					( i < bitmap.width && j < bitmap.rows )
					? bitmap.buffer[ i + bitmap.width * j ]
					: 0;
			}
		}

		glBindTexture( GL_TEXTURE_2D, texture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expandedData );

		glNewList( (listBase + c), GL_COMPILE );
			glBindTexture( GL_TEXTURE_2D, texture );
			glTranslatef( float(bitmapGlyph->left), 0.0f, 0.0f );
			glPushMatrix();
			glTranslatef( 0.0f, float(bitmapGlyph->top - bitmap.rows), 0.0f );

			const float x = float(bitmap.width) / float(width);
			const float y = float(bitmap.rows)  / float(height);

			glBegin( GL_QUADS );
				glTexCoord2f( 0.0f, 0.0f );  glVertex2i( 0,            bitmap.rows );
				glTexCoord2f( 0.0f, y    );  glVertex2i( 0,            0           );
				glTexCoord2f( x,    y    );  glVertex2i( bitmap.width, 0           );
				glTexCoord2f( x,    0.0f );  glVertex2i( bitmap.width, bitmap.rows );
			glEnd();
			glPopMatrix();

			fontWidth = face->glyph->advance.x >> 6;
			glTranslatef( float(fontWidth), 0.0f, 0.0f );
		glEndList();
		return true;
	}
}


///////////////////////////////////////////////////////////////////////////////////
GlFont::GlFont( void )
	: m_height( -1.0f )
	, m_base( 0 )
{
	memset( m_textures, 0, sizeof(m_textures) );
	memset( m_widths, 0, sizeof(m_widths) );
}

///////////////////////////////////////////////////////////////////////////////////
GlFont::~GlFont( void )
{
	Release();
}

///////////////////////////////////////////////////////////////////////////////////
bool GlFont::Load( const std::string& fontname, int height )
{
	FT_Library lib;
	FT_Face face;

	if( FT_Init_FreeType( &lib ) )
	{
		logError( "Failed to initialize FreeType library" );
		return false;
	}

	if( FT_New_Face( lib, fontname.c_str(), 0, &face ) )
	{
		logError( "Failed to initialize font face for '%s'", fontname.c_str() );
		FT_Done_FreeType( lib );
		return false;
	}

	m_height = float(height) / 0.63f;

#if 1
	FT_Set_Char_Size( face, height<<6, height<<6, 96, 96 );
#else
	if( FT_Set_Pixel_Sizes( face, 0, height ) )
	{
		logError( "Failed to set font '%s' to pixel size %ix%i", fontname.c_str(), height, height );
		FT_Done_Face( face );
		FT_Done_FreeType( lib );
		return false;
	}
#endif

	m_base = glGenLists( 128 );
	glGenTextures( 128, m_textures );

	for( int i = 0; i < 128; ++i )
	{
		if( ! _CreateCharacter( face, i, m_base, m_textures[i], m_widths[i] ) )
		{
			FT_Done_Face( face );
			FT_Done_FreeType( lib );
			return false;
		}
	}

	FT_Done_Face( face );
	FT_Done_FreeType( lib );

	logDebug( "Loaded font '%s'", fontname.c_str() );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////
void GlFont::Release( void )
{
	if( m_base )
	{
		glDeleteLists( m_base, 128 );
		glDeleteTextures( 128, m_textures );

		memset( m_textures, 0, sizeof(m_textures) );
		memset( m_widths, 0, sizeof(m_widths) );

		m_height = -1;
		m_base = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////
unsigned GlFont::width( const std::string& text )
{
	unsigned sum = 0.0f;
	for( size_t i=0; i < text.size(); ++i )
	{
		if( text[i] > 0 && text[i] < 128 )
			sum += m_widths[ (int) text[i] ];
	}
	return sum;
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
GlPrinter::GlPrinter( GlFont& font )
	: m_font( font )
{
	// nothing
}

///////////////////////////////////////////////////////////////////////////////////
GlPrinter::~GlPrinter( void )
{
	// nothing
}

///////////////////////////////////////////////////////////////////////////////////
void GlPrinter::Print( float x, float y, const std::string& text )
{
	glPushAttrib( GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT );

	glMatrixMode( GL_MODELVIEW );
	glDisable( GL_LIGHTING );
	glEnable( GL_TEXTURE_2D );
	glDisable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glListBase( m_font.list_base() );

	float modelview[16];
	glGetFloatv( GL_MODELVIEW_MATRIX, modelview );

	glPushMatrix();
	glLoadIdentity();
	glTranslatef( x, y, 0.0f );
	glMultMatrixf( modelview );

	glCallLists( text.size(), GL_UNSIGNED_BYTE, text.c_str() );

	glPopMatrix();
	glPopAttrib();
}

///////////////////////////////////////////////////////////////////////////////////
void GlPrinter::Print( int x, int y, const std::string& text )
{
	glPushAttrib( GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT );

	glMatrixMode( GL_MODELVIEW );
	glDisable( GL_LIGHTING );
	glEnable( GL_TEXTURE_2D );
	glDisable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glListBase( m_font.list_base() );

	float modelview[16];
	glGetFloatv( GL_MODELVIEW_MATRIX, modelview );

	glPushMatrix();
	glLoadIdentity();
	glTranslatef( x, y, 0.0f );
	glMultMatrixf( modelview );

	glCallLists( text.size(), GL_UNSIGNED_BYTE, text.c_str() );

	glPopMatrix();
	glPopAttrib();
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

GlLayout::GlLayout( int width, int height )
	: m_width( width )
	, m_height( height )
{
	GLint viewport[4];

	glPushAttrib( GL_TRANSFORM_BIT );
	glGetIntegerv( GL_VIEWPORT, viewport );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D
		( viewport[0]
		, viewport[0]+width
		, viewport[1]
		, viewport[1]+height
		);

	glPopAttrib();
}

///////////////////////////////////////////////////////////////////////////////
GlLayout::GlLayout( void )
	: m_width( 0 )
	, m_height( 0 )
{
	GLint viewport[4];

	glPushAttrib( GL_TRANSFORM_BIT );
	glGetIntegerv( GL_VIEWPORT, viewport );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D
		( viewport[0]
		, viewport[2]
		, viewport[1]
		, viewport[3]
		);

	m_width  = ( viewport[2] - viewport[0] );
	m_height = ( viewport[3] - viewport[1] );

	glPopAttrib();
}

///////////////////////////////////////////////////////////////////////////////
GlLayout::~GlLayout( void )
{
	glPushAttrib( GL_TRANSFORM_BIT );
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glPopAttrib();
}

