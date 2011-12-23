// score.cpp

#include "score.h"
#include "log.h"
#include "board.h"
#include <set>
#include "assert.h"

const unsigned POINTS_PER_GEM = 50;

////////////////////////////////////////////////////////////////////////////////
Score::Score( void )
	: m_score( 0 )
	, m_wave( 0 )
	, m_cleared()
	, m_matches()
{
	Reset();
}

////////////////////////////////////////////////////////////////////////////////
Score::~Score( void )
{
	// nothing
}

////////////////////////////////////////////////////////////////////////////////
void Score::Reset( void )
{
	m_score = 0;
	m_matches.clear();
	m_wave = 0;

	m_cleared.resize( 32 );
	for( size_t i = 0; i < m_cleared.size(); ++i )
		m_cleared[i] = 0;
}

////////////////////////////////////////////////////////////////////////////////
void Score::MarkIdle( void )
{
	m_matches.clear();
	m_wave = 0;
}

////////////////////////////////////////////////////////////////////////////////
void Score::BeginMatching( void )
{
	++m_wave;
	m_matches.clear();
}

////////////////////////////////////////////////////////////////////////////////
void Score::EndMatching( void )
{
	if( m_matches.empty() )
	{
		m_wave = 0;
	}
	else
	{
		std::set< const Gem* > matchSet;
		for( unsigned m=0; m < m_matches.size(); ++m )
		{
			for( unsigned g=0; g < m_matches[m].size(); ++g )
				matchSet.insert( m_matches[m][g] );
		}

		std::set< const Gem* >::iterator iter;
		for( iter = matchSet.begin(); iter != matchSet.end(); iter++ )
		{
			ASSERT( (*iter)->type < int(m_cleared.size()) );
			m_cleared[ (*iter)->type ] ++;
		}

		unsigned amount = ( m_wave * m_matches.size() * matchSet.size() ) * POINTS_PER_GEM;
		m_score += amount;
		logInfo( "Score: total=%u amount=%u wave=%u matches=%u gems=%u"
			, m_score
			, amount
			, m_wave
			, (unsigned) m_matches.size()
			, (unsigned) matchSet.size() );
	}
}

////////////////////////////////////////////////////////////////////////////////
void Score::Add( const std::vector< const Gem* >& gem )
{	
	m_matches.push_back( gem );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
ScoreKeeper::ScoreKeeper( Score& score )
	: m_score( score )
	, m_matches()
{
	// nothing
}

////////////////////////////////////////////////////////////////////////////////
ScoreKeeper::~ScoreKeeper( void )
{
	Flush();
}

////////////////////////////////////////////////////////////////////////////////
void ScoreKeeper::Add( const Gem* g )
{
	if( g )
		m_matches.push_back( g );
}

////////////////////////////////////////////////////////////////////////////////
void ScoreKeeper::Flush( void )
{
	if( m_matches.size() )
	{
		m_score.Add( m_matches );
		m_matches.clear();
	}
}


