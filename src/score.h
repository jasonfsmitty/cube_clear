#ifndef SCORE_H
#define SCORE_H

#include <stdint.h>
#include <vector>

class Gem;

class Score
{
	public:
		Score( void );
		~Score( void );

		unsigned value( void ) const { return m_score; }

		void Reset( void );

		void BeginMatching( void );
		void EndMatching( void );

		void Add( const std::vector< const Gem* >& gem );

		void MarkIdle( void );

	private:

		unsigned m_score;
		unsigned m_wave;

		std::vector< std::vector< const Gem* > > m_matches;
};

class ScoreKeeper
{
	public:
		ScoreKeeper( Score& score );
		~ScoreKeeper( void );

		void Add( const Gem* g );
		void Flush( void );

	private:
		Score& m_score;
		std::vector< const Gem* > m_matches;
};

#endif /* SCORE_H */
