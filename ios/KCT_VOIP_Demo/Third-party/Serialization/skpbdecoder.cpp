
#include "skpbdecoder.h"
#include "skpbhelper.h"

#include "tlvport.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#define return for( LogErr("return %s %d\n",__FILE__,__LINE__); ; ) return  

namespace Comm
{

enum
{
	SKPBNoTag = -1,
	SKPBBadTag = -2
};


SKPBDecoder :: SKPBDecoder ( )
{
	m_pcOrigHeader = NULL;
	m_pcBuffer = NULL;
}

SKPBDecoder :: ~SKPBDecoder ()
{
	m_pcOrigHeader = NULL;
	if( m_pcBuffer != NULL )
	{
		free( m_pcBuffer );
		m_pcBuffer = NULL;
	}
}

int SKPBDecoder :: Attach( char * apcBuf, const int &aiSize )
{
	return InitObject( apcBuf, aiSize );
}

int SKPBDecoder  :: GetTag()
{
	if( m_iTag != SKPBNoTag )
		return m_iTag;

	unsigned long long key = 0 ;

	m_iUnReadPos = 0;
	int ret = GetValue( key );
	if( ret ) 
		return -1;

	m_iKeySize = m_iUnReadPos;
	m_iUnReadPos = 0;

	m_iTag = key >> SKPBHelper :: TYPEBIT;

	m_iType = key & SKPBHelper :: TYPEMASK;

	return 0;
}

int SKPBDecoder :: GetType()
{
	return m_iType;
}

int SKPBDecoder :: GetTextLenWithLenField()
{
	m_iUnReadPos = 0;

	int ret = GetValue( m_iVarintLen );
	if( ret || m_iVarintLen < 0 )
		return -1;
	
	m_iKeySize += m_iUnReadPos;
	m_iUnReadPos = 0;
	
	return 0;
}

int SKPBDecoder :: GetTextLenWithOutLenField( )
{
	m_iVarintLen = 0;

	while( 1 )
	{
		if( m_iKeySize >= m_iSize )
		{
//			TLV_LOG_ERR("%s::ERROR size not match m_iKeySize(%d) m_iSize(%d)",__FUNCTION__,m_iKeySize, m_iSize );
			return -1;
		}

		char tmp = *( m_pcBuffer + m_iKeySize + m_iVarintLen );
		m_iVarintLen += sizeof( tmp );

		if( !(tmp & 0x80) )
		{
			break;
		}
	}

	if( m_iKeySize > m_iSize )
	{
//		TLV_LOG_ERR("%s::ERROR size not match m_iKeySize(%d) m_iSize(%d)",__FUNCTION__,m_iKeySize, m_iSize );
		return -1;
	}

	return 0;
}

int SKPBDecoder :: Reset()
{
	return InitObject( m_pcOrigHeader, m_iOrigSize );
}

int SKPBDecoder :: InitObject( char * apcBuf, const int &aiSize )
{
	if( aiSize < 0 || apcBuf == NULL )
	{
		return -1;
	}

	if( aiSize == 0 )
	{
		m_iTag = -2;
		return SKPB_EMPTYSTRUCT;
	}

	m_pcBuffer = apcBuf;
	
	if( m_pcOrigHeader == NULL )
	{
		m_pcOrigHeader = apcBuf;
		m_iOrigSize = aiSize;
	}
	
	m_iSize = aiSize;
	m_iTag = SKPBNoTag;
	m_iType = SKPBNoTag;
	m_iUnReadPos = 0;
	m_iVarintLen = 0;
	m_iKeySize = 0;

	GetTag();

	if( ( m_iType ^ SKPBHelper :: PBTYPE_LENLIMITED ) == 0 )
	{
		if( GetTextLenWithLenField() )
			return -1;
	}
	else if( ( m_iType ^ SKPBHelper :: PBTYPE_64BIT ) == 0 )
	{
		//double
		m_iVarintLen = sizeof( double );
	}
	else if( ( m_iType ^ SKPBHelper :: PBTYPE_32BIT ) == 0 )
	{
		//float
		m_iVarintLen = sizeof( float );
	}
	else 
	{
		//varints 
		if( GetTextLenWithOutLenField() )
			return -1;
	}

	if ( m_iVarintLen + m_iKeySize > m_iSize )
	{
		TLV_LOG_ERR("%s::Error size error varintlen(%d) keysize(%d) totsie(%d)", 
				__FUNCTION__, m_iVarintLen, m_iKeySize, m_iSize );
		return -1;
	}

	return 0;
}
int SKPBDecoder :: GetNextObject()
{
	return InitObject( m_pcBuffer + m_iKeySize + m_iVarintLen, m_iSize - m_iKeySize -m_iVarintLen );
}

int SKPBDecoder :: GetSubObject( SKPBDecoder * apoBuffer )
{
	if( m_iType != SKPBHelper :: PBTYPE_LENLIMITED )
	{
		return -1;
	}
	return apoBuffer -> Attach( m_pcBuffer + m_iKeySize, m_iVarintLen );
}

char * SKPBDecoder :: GetString( const int & aiLen )
{
	if( m_iKeySize + m_iUnReadPos + aiLen > m_iSize )
	{
		TLV_LOG_ERR("%s::Error size error varintlen(%d) keysize(%d) totsie(%d)", 
				__FUNCTION__, m_iVarintLen, m_iKeySize, m_iSize );
		return NULL;
	}

	char * pcValue = m_pcBuffer + m_iKeySize + m_iUnReadPos; 
	m_iUnReadPos += aiLen;

	return pcValue;
}

//************************get value method***********************


int SKPBDecoder :: GetInt64(  long long & alValue )
{
	if( m_iType != SKPBHelper :: PBTYPE_VARINTTYPE )
	{
		TLV_LOG_ERR("%s::Error type error type %d",__FUNCTION__, m_iType );
		return -1;
	}
	return GetValue( alValue );
}

int SKPBDecoder :: GetUInt64(  unsigned long long & alValue )
{
	if( m_iType != SKPBHelper :: PBTYPE_VARINTTYPE )
	{
		TLV_LOG_ERR("%s::Error type error type %d",__FUNCTION__, m_iType );
		return -1;
	}

	return GetValue( alValue );
}

int SKPBDecoder :: GetSInt32(  int & aiValue )
{
	if( m_iType != SKPBHelper :: PBTYPE_VARINTTYPE )
	{
		TLV_LOG_ERR("%s::Error type error type %d",__FUNCTION__, m_iType );
		return -1;
	}

	int ret = GetValue( aiValue );
	if( ret ) 
		return -1;
	aiValue = SKPBHelper :: Decode32( aiValue );
	return 0;
}

int SKPBDecoder :: GetSInt64(  long long & alValue )
{
	if( m_iType != SKPBHelper :: PBTYPE_VARINTTYPE )
	{
		TLV_LOG_ERR("%s::Error type error type %d",__FUNCTION__, m_iType );
		return -1;
	}

	int ret = GetValue( alValue );
	if( ret ) 
		return -1;
	alValue = SKPBHelper :: Decode64( alValue );
	return 0;
}

template < typename T >
int SKPBDecoder :: GetValue( T &aValue )
{
	aValue = 0;
	
	int iShiftPos = 0;
	while( 1 )
	{
		if( m_iKeySize + m_iUnReadPos >= m_iSize )
		{
			TLV_LOG_ERR("%s::Error size error nowvarintpos(%d) keysize(%d) totsie(%d)", 
					__FUNCTION__, m_iUnReadPos, m_iKeySize, m_iSize );
			return -1;
		}

		char tmp = *( m_pcBuffer + m_iKeySize + m_iUnReadPos );
		m_iUnReadPos += sizeof( tmp );
		
		unsigned long long ull= ( unsigned long long )(tmp & 0x7f ) << iShiftPos; 
		aValue |= ull;

		iShiftPos += 7;

		if( !(tmp & 0x80) )
		{
			break;
		}
	}

	if( m_iKeySize + m_iUnReadPos > m_iSize )
	{
		TLV_LOG_ERR("%s::Error size error nowvarintpos(%d) keysize(%d) totsie(%d)", 
				__FUNCTION__, m_iUnReadPos, m_iKeySize, m_iSize );
		return -1;
	}

	return 0;
}

int SKPBDecoder :: GetFloat( float & afValue )
{
	if( ( m_iType ^ SKPBHelper :: PBTYPE_32BIT )  )
	{
		return -1;
	}

	char * pcValue = GetString( sizeof( afValue) );

	if( pcValue == NULL )
		return -1;

	memcpy( &afValue, pcValue, sizeof( afValue ) );
	return 0;
}

int SKPBDecoder :: GetDouble( double & adValue )
{
	if( ( m_iType ^ SKPBHelper :: PBTYPE_64BIT )  )
	{
		return -1;
	}

	char * pcValue = GetString( sizeof( adValue) );

	if( pcValue == NULL )
		return -1;

	memcpy( &adValue, pcValue, sizeof( adValue ) );

	return 0;
}


int SKPBDecoder :: Detach( )
{
	m_pcBuffer = NULL;
	return 0;
}

//for string
int SKPBDecoder :: GetStringArrayLen( const int &aiTag, int &aiLen )
{
	if( m_iType ^ SKPBHelper :: PBTYPE_LENLIMITED )
		return -1;
	
	aiLen = m_iVarintLen;
	return 0;
}

int SKPBDecoder :: GetArray( float *apfArray, const int &aiLen )
{
	if( m_iType ^ SKPBHelper :: PBTYPE_LENLIMITED )
		return -1;
	m_iType = SKPBHelper :: PBTYPE_32BIT;
	for( int i = 0;i < aiLen; ++i )
	{
		if( GetFloat( apfArray[i] ) )
			return -1;
	}
	m_iType = SKPBHelper :: PBTYPE_LENLIMITED;
	return 0;
}

int SKPBDecoder :: GetArray( double *apdArray, const int &aiLen )
{
	if( m_iType ^ SKPBHelper :: PBTYPE_LENLIMITED )
		return -1;
	m_iType = SKPBHelper :: PBTYPE_64BIT;
	for( int i = 0;i < aiLen; ++i )
	{
		if( GetDouble( apdArray[i] ) )
			return -1;
	}
	m_iType = SKPBHelper :: PBTYPE_LENLIMITED;
	return 0;
}

int SKPBDecoder :: GetArray( char *apcArray, const int &aiLen )
{
	if( m_iType ^ SKPBHelper :: PBTYPE_LENLIMITED )
		return -1;
	char * ptr = GetString( aiLen );
	if( ptr == NULL ) 
		return -1;
	memcpy( apcArray, ptr, aiLen );
	return 0;
}

int SKPBDecoder :: GetArray( unsigned char *apcArray, const int &aiLen )
{
	if( m_iType ^ SKPBHelper :: PBTYPE_LENLIMITED )
		return -1;
	char * ptr = GetString( aiLen );
	if( ptr == NULL ) 
		return -1;
	memcpy( (char *)apcArray, ptr, aiLen );
	return 0;
}

int SKPBDecoder :: GetSInt32Array( int *apiArray, const int &aiLen )
{
	if( m_iType ^ SKPBHelper :: PBTYPE_LENLIMITED )
		return -1;
	for( int i = 0;i < aiLen; i ++ )
	{
		if( GetSInt32( apiArray[i] ) )
			return -1;	
	}
	return 0;
}

int SKPBDecoder :: GetSInt64Array( long long *aplArray, const int &aiLen )
{
	if( m_iType ^ SKPBHelper :: PBTYPE_LENLIMITED )
		return -1;
	for( int i = 0;i < aiLen; i ++ )
	{
		if( GetSInt64( aplArray[i] ) )
			return -1;	
	}
	return 0;
}

int SKPBDecoder :: GetArray( short *aphArray, const int &aiLen )
{
	return GetArrayValue( aphArray, aiLen );
}
int SKPBDecoder :: GetArray( unsigned short *aphArray, const int &aiLen )
{
	return GetArrayValue( aphArray, aiLen );
}
int SKPBDecoder :: GetArray( int *apiArray, const int &aiLen )
{
	return GetArrayValue( apiArray, aiLen );
}
int SKPBDecoder :: GetArray( unsigned int *apiArray, const int &aiLen )
{
	return GetArrayValue( apiArray, aiLen );
}
int SKPBDecoder :: GetArray( long long *aplArray, const int &aiLen )
{
	return GetArrayValue( aplArray, aiLen );
}
int SKPBDecoder :: GetArray( unsigned long long *aplArray, const int &aiLen )
{
	return GetArrayValue( aplArray, aiLen );
}



template < typename T > 
int SKPBDecoder :: GetArrayValue( T *apArray, const int &iSize )
{
	if( m_iType ^ SKPBHelper :: PBTYPE_LENLIMITED )
		return -1;
	for( int i = 0;i < iSize; i ++ )
	{
		if( GetValue( apArray[i] ) )
			return -1;	
	}
	return 0;
}

}

