#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "worker.h"
#include "defines.h"
#include <vector>

class CubeBackground : public Worker
{
	public:
		CubeBackground( void );
		~CubeBackground( void );

		virtual Worker::Status Update( float deltaTime );
		virtual void Render( void );

		virtual Worker::Status Handle( const SDL_Event& event )
			{ return Worker::Continue; }

		virtual bool IsAlive( void )
			{ return true; }

		virtual void Pause( void )
			{}

		virtual void Resume( void );

		struct Block
		{
			float center[3];
			float velocity;
			float orientation[3];
			float spin[3];
			float color[4];
			float size;
		};

		struct Config
		{
			float minVel;
			float maxVel;
			float minSpin;
			float maxSpin;
			float minSize;
			float maxSize;

			float color[4];
			float bgColor[4];

			float area;
			unsigned numBlocks;
		};

	private:
		std::vector< Block > m_blocks;
		Config m_config;
		float m_angle;
};

#endif /* BACKGROUND_H */

