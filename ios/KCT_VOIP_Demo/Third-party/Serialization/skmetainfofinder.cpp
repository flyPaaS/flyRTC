
#include "skmetainfofinder.h"
#include <algorithm>

#include <string.h>

using namespace std;

namespace Comm
{

typedef struct tagSKMetaInfoFinderImpl 
{
	SKMetaInfoFinder :: SKStructInfo_t *m_ptStructInfo;
	int * m_piStructIndex;
	
	int m_iMinStructID;
	int m_iMaxStructID;

	int m_iStructNum;

}SKMetaInfoFinderImpl_t;

int SKMetaInfoFinder :: SKStructInfo_t :: FindFieldIndex( const  int &iId ) const
{
	if( iId > iMaxFieldID || iId < iMinFieldID )
		return -1;
	return piFieldListIndex[ iId - iMinFieldID ] ;
}

SKMetaInfoFinder :: SKMetaInfoFinder ( const SKMetaInfo_t * ptMetaInfo )
{
	m_Impl = new SKMetaInfoFinderImpl_t;
	Init( ptMetaInfo );
}

SKMetaInfoFinder :: ~SKMetaInfoFinder ( )
{
	for(int i = 0; i < m_Impl -> m_iStructNum; ++i )
	{
		free( m_Impl -> m_ptStructInfo[i].piFieldListIndex );
	}

	if( m_Impl -> m_piStructIndex )
	{
		free( m_Impl -> m_piStructIndex );
		m_Impl -> m_piStructIndex = NULL;
	}

	if( m_Impl -> m_ptStructInfo )
	{
		free(m_Impl -> m_ptStructInfo);
		m_Impl -> m_ptStructInfo = NULL;
	}
	m_Impl -> m_iStructNum = 0;
	if( m_Impl )
	{
		delete m_Impl;
		m_Impl= NULL;
	}
}

void SKMetaInfoFinder :: Init( const Comm::SKMetaInfo_t * ptMetaInfo )
{
	m_Impl -> m_iStructNum = ptMetaInfo -> hStructCount;
	
	m_Impl -> m_iMaxStructID = 0;
	m_Impl -> m_iMinStructID = 0x7fffffff;

	for( int i = 0; i < m_Impl -> m_iStructNum; ++i )
	{
		SKMetaStruct_const *ptMetaStruct = &( ptMetaInfo -> ptStructList[i] );
		m_Impl -> m_iMaxStructID = max( m_Impl -> m_iMaxStructID , (int)ptMetaStruct -> hId );
		m_Impl -> m_iMinStructID = min( m_Impl -> m_iMinStructID , (int)ptMetaStruct -> hId );
	}
	
	m_Impl -> m_ptStructInfo = ( SKStructInfo_t * ) malloc( sizeof( SKStructInfo_t ) * m_Impl -> m_iStructNum );
	
	int iIndexNum = m_Impl -> m_iMaxStructID - m_Impl -> m_iMinStructID + 1;

	m_Impl -> m_piStructIndex = ( int * ) malloc( sizeof(int) * ( iIndexNum ) );

	memset( m_Impl -> m_piStructIndex ,-1, sizeof(int) * ( iIndexNum ) );

	for(int i=0;i < m_Impl -> m_iStructNum; ++i)
	{
		SKMetaStruct_const *ptMetaStruct = &( ptMetaInfo -> ptStructList[i] );

		m_Impl -> m_ptStructInfo[i].ptStruct = ptMetaStruct;
		m_Impl -> m_piStructIndex[ ptMetaStruct->hId - m_Impl -> m_iMinStructID ] = i;

		m_Impl -> m_ptStructInfo[i].iMaxFieldID = 0;
		m_Impl -> m_ptStructInfo[i].iMinFieldID = 0x7fffffff;
		
		for( int j = 0;  j < ptMetaStruct -> hFieldCount; ++j )
		{
			SKMetaField_const * ptMetaField = &( ptMetaStruct -> ptFieldList[j] );

			m_Impl -> m_ptStructInfo[i].iMaxFieldID = 
						max( m_Impl -> m_ptStructInfo[i].iMaxFieldID, (int)ptMetaField -> hId ) ;

			m_Impl -> m_ptStructInfo[i].iMinFieldID = 
						min( m_Impl -> m_ptStructInfo[i].iMinFieldID, (int)ptMetaField -> hId ) ;
		}

		int iFieldRange = m_Impl -> m_ptStructInfo[i].iMaxFieldID - m_Impl -> m_ptStructInfo[i].iMinFieldID + 1;
		if( iFieldRange > 0 )
		{
			m_Impl -> m_ptStructInfo[i].piFieldListIndex = (int *)malloc( sizeof(int) * ( iFieldRange ) );
			memset( m_Impl -> m_ptStructInfo[i].piFieldListIndex, -1, sizeof(int) * ( iFieldRange ) );

			for( int j = 0;  j < ptMetaStruct -> hFieldCount; ++j )
			{
				SKMetaField_const * ptMetaField = &(ptMetaStruct -> ptFieldList[j]);
				m_Impl -> m_ptStructInfo[i].piFieldListIndex[ ptMetaField -> hId -  m_Impl -> m_ptStructInfo[i].iMinFieldID ] = j;
			}
		}
	}
	
}

const SKMetaInfoFinder :: SKStructInfo_t * SKMetaInfoFinder :: GetStructInfo ( const int &iTypeID )const
{
	int iIndex = FindStructIndex( iTypeID );
	
	if( iIndex == -1 )
		return NULL;

	return m_Impl -> m_ptStructInfo+iIndex;
}

int SKMetaInfoFinder :: FindStructIndex( const int &iTypeID ) const
{
	if( iTypeID > m_Impl-> m_iMaxStructID || iTypeID < m_Impl -> m_iMinStructID ) 
		return -1;
	return m_Impl -> m_piStructIndex[ iTypeID - m_Impl -> m_iMinStructID ];
}

}

