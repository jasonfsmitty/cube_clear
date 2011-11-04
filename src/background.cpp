#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

#include "background.h"
#include "defines.h"

namespace block {

	///////////////////////////////////////////////////////////////////////
	void Config( CubeBackground::Config& config )
	{
		config.color[0] = 0.00f;
		config.color[1] = 0.50f;
		config.color[2] = 0.60f;
		config.color[3] = 0.25f;

		config.bgColor[0] = 0.0f;
		config.bgColor[1] = 0.0f;
		config.bgColor[2] = 0.1f;
		config.bgColor[3] = 0.0f;

		config.minVel = 1.0f;
		config.maxVel = 2.5f;
		config.minSpin = 45.0f;
		config.maxSpin = 100.0f;
		config.minSize = 0.25f;
		config.maxSize = 1.25f;

		config.area = 7.5f;
		config.numBlocks = 500;
	}

	///////////////////////////////////////////////////////////////////////
	void Init
		( const CubeBackground::Config& config
		, CubeBackground::Block& block
		, bool initial
		)
	{
		block.center[0] = utils::randf( -(config.area), config.area );
		block.center[1] = (initial ? utils::randf( -(config.area), config.area ) : config.area );
		block.center[2] = utils::randf( -5.0, 0.0f );

		block.velocity = utils::randf( config.minVel, config.maxVel );

		block.spin[0] = utils::randf( config.minSpin, config.maxSpin );
		block.spin[1] = utils::randf( config.minSpin, config.maxSpin );
		block.spin[2] = utils::randf( config.minSpin, config.maxSpin );

		block.orientation[0] = utils::randf( 0.0f, 360.0f );
		block.orientation[1] = utils::randf( 0.0f, 360.0f );
		block.orientation[2] = utils::randf( 0.0f, 360.0f );

		block.color[0] = config.color[0];
		block.color[1] = config.color[1];
		block.color[2] = config.color[2];
		block.color[3] = config.color[3];

		block.size = utils::randf( config.minSize, config.maxSize );
	}

	///////////////////////////////////////////////////////////////////////
	void Update
		( const CubeBackground::Config& config
		, CubeBackground::Block& block
		, float deltaTime
		)
	{
		block.center[1] -= deltaTime * block.velocity;

		// check if still valid
		if( block.center[1] < -(config.area) )
		{
			Init( config, block, false );
			return;
		}

		// update the orientation..
		for( int i=0; i<3; i++ )
		{
			block.orientation[i] += block.spin[i] * deltaTime;
			if( block.orientation[i] > 360.0f )
				block.orientation[i] -= 360.0f;
		}
	}

	///////////////////////////////////////////////////////////////////////
	void DrawCube( float texStart, float texEnd, float size )
	{
		const float cubeNormals[6][3] =
		{
			{  0.0f,  0.0f,  1.0f }, // front face
			{  0.0f,  0.0f, -1.0f }, // back
			{  0.0f,  1.0f,  0.0f }, // top
			{  0.0f, -1.0f,  0.0f }, // bottom
			{ -1.0f,  0.0f,  0.0f }, // left
			{  1.0f,  0.0f,  0.0f }  // right
		};

		size = size*0.5f;

		glBegin( GL_TRIANGLE_STRIP );
			// right face first
			glNormal3fv( cubeNormals[5] );
			glTexCoord2f( texStart, texStart );	glVertex3f( size, size, size );
			glTexCoord2f( texEnd, texStart );	glVertex3f( size, size, -size );
			glTexCoord2f( texStart, texEnd );	glVertex3f( size, -size, size );
			glTexCoord2f( texEnd, texEnd );		glVertex3f( size, -size, -size );
		glEnd();

		glBegin( GL_TRIANGLE_STRIP );
			// left face
			glNormal3fv( cubeNormals[4] );		
			glTexCoord2f( texStart, texStart );	glVertex3f( -size, size, -size );
			glTexCoord2f( texEnd, texStart );	glVertex3f( -size, size, size );
			glTexCoord2f( texStart, texEnd );	glVertex3f( -size, -size, -size );
			glTexCoord2f( texEnd, texEnd );		glVertex3f( -size, -size, size );
		glEnd();		

		glBegin( GL_TRIANGLE_STRIP );
			// top
			glNormal3fv( cubeNormals[2] );
			glTexCoord2f( texStart, texStart );	glVertex3f( -size, size, -size );
			glTexCoord2f( texEnd, texStart );	glVertex3f( size, size, -size ); 
			glTexCoord2f( texStart, texEnd );	glVertex3f( -size, size, size );
			glTexCoord2f( texEnd, texEnd );		glVertex3f( size, size, size );
		glEnd();

		glBegin( GL_TRIANGLE_STRIP );
			// bottom
			glNormal3fv( cubeNormals[3]);
			glTexCoord2f( texStart, texStart );	glVertex3f( -size, -size, size ); 
			glTexCoord2f( texEnd, texStart );	glVertex3f( size, -size, size ); 
			glTexCoord2f( texStart, texEnd );	glVertex3f( -size, -size, -size );
			glTexCoord2f( texEnd, texEnd );		glVertex3f( size, -size, -size );		
		glEnd();

		glBegin( GL_TRIANGLE_STRIP );
			// front
			glNormal3fv( cubeNormals[0]);
			glTexCoord2f( texStart, texStart );	glVertex3f( -size, size, size );
			glTexCoord2f( texEnd, texStart );	glVertex3f( size, size, size );
			glTexCoord2f( texStart, texEnd );	glVertex3f( -size, -size, size );
			glTexCoord2f( texEnd, texEnd );		glVertex3f( size, -size, size );
		glEnd();
		
		glBegin( GL_TRIANGLE_STRIP );
			// back
			glNormal3fv( cubeNormals[1] );
			glTexCoord2f( texStart, texStart );	glVertex3f( size, size, -size);
			glTexCoord2f( texEnd, texStart );	glVertex3f( -size, size, -size );
			glTexCoord2f( texStart, texEnd );	glVertex3f( size, -size, -size );
			glTexCoord2f( texEnd, texEnd );		glVertex3f( -size, -size, -size );
		glEnd();
	}

	///////////////////////////////////////////////////////////////////////
	void Draw( CubeBackground::Block& block )
	{
		glPushMatrix();
		glTranslatef( block.center[0], block.center[1], block.center[2] );
		glRotatef( block.orientation[0], 1.0f, 0.0f, 0.0f );
		glRotatef( block.orientation[1], 0.0f, 1.0f, 0.0f );
		glRotatef( block.orientation[2], 0.0f, 0.0f, 1.0f );
		glColor4fv( block.color );
		DrawCube( 0.0f, 1.0f, block.size );
		glPopMatrix();
	}
};

///////////////////////////////////////////////////////////////////////////////
CubeBackground::CubeBackground( void )
	: m_blocks()
	, m_config()
{
	block::Config( m_config );

	m_blocks.resize( m_config.numBlocks );
	for( unsigned i=0; i < m_blocks.size(); i++ )
		block::Init( m_config, m_blocks[i], true );

	glClearColor( m_config.bgColor[0], m_config.bgColor[1], m_config.bgColor[2], m_config.bgColor[3] );
}

///////////////////////////////////////////////////////////////////////////////
CubeBackground::~CubeBackground( void )
{
	// nothing
}

///////////////////////////////////////////////////////////////////////////////
Worker::Status CubeBackground::FrameUpdate( float deltaTime )
{
	glPushAttrib( GL_COLOR_BUFFER_BIT );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();

	gluPerspective( 45.0f,
					4.0f / 3.0f,
					4.0f,
					40.0f   );
#if 0
	static float angle = utils::randf( 0.0f, 360.0f );

	angle += deltaTime * 9.0f;

	// setup the viewing matrix...
	gluLookAt( 0.0f, 0.0f, 7.0f,  // eye
		       0.0f, 0.0f, 0.0f,   // center
			   cos(angle * 3.24f/180.0f), sin(angle*3.14f/180.0f), 0.0f ); // up
#else
	// setup the viewing matrix...
	gluLookAt( 0.0f, 0.0f, 10.0f,  // eye
		       0.0f, 0.0f, 0.0f,   // center
			   0.0f, 1.0f, 0.0f ); // up
#endif

	glMatrixMode( GL_MODELVIEW );
	glBindTexture( GL_TEXTURE_2D, 0 );

	for( unsigned i=0; i < m_blocks.size(); ++i )
	{
		Block& block = m_blocks[i];
		block::Update( m_config, block, deltaTime );
		block::Draw( block );
	}

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

	glPopAttrib();

	return Worker::Continue;
}

