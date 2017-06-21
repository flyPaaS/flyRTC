
#include "skpbencoder.h"
#include "skpbhelper.h"

#include <stdlib.h>
#include <string.h>

namespace Comm
{

SKPBEncoder :: SKPBEncoder ( const int &aiSize )
{
	m_pcBuffer = ( char * )malloc ( sizeof ( char ) * aiSize ) ;
	m_iSize = aiSize;
	m_iUsedSize = 0;
	memset( m_pcBuffer , 0 , m_iSize );
}

SKPBEncoder :: ~SKPBEncoder ()
{
	if( m_pcBuffer != NULL )
	{
		free( m_pcBuffer );
		m_pcBuffer = NULL;
	}
}

int SKPBEncoder :: Detach( char ** apcBuf, int * aiLen )
{
	if( apcBuf == NULL )
	{
		return -1;
	}
	
	*aiLen = m_iUsedSize;

	*apcBuf = m_pcBuffer;
	
	m_pcBuffer = NULL;

	return 0;

}


//*****************add value method *************************

int SKPBEncoder :: AddInt64( const int &aiTag, const long long &alValue)
{
	return AddValue( aiTag, alValue );
}

int SKPBEncoder :: AddUInt64( const int &aiTag, const unsigned long long &alValue)
{
	return AddValue( aiTag, alValue );
}

int SKPBEncoder :: AddSInt32( const int &aiTag, const int &alValue)
{
	unsigned int iEncodeValue = SKPBHelper :: Encode32( alValue );
	return AddValue( aiTag, iEncodeValue );
}

int SKPBEncoder :: AddSInt64( const int &aiTag, const long long &alValue)
{
	unsigned long long iEncodeValue = SKPBHelper :: Encode64( alValue );
	return AddValue( aiTag, iEncodeValue );
}

template < typename T >
int SKPBEncoder :: AddValue( const int &aiTag, const T &aValue )
{
	int ret = AddKey( aiTag, SKPBHelper :: PBTYPE_VARINTTYPE );

	ret |= AddValue( aValue );

	return ret;
}

int SKPBEncoder :: AddFloat( const int &aiTag, const float &adValue)
{

	int ret = AddKey( aiTag, SKPBHelper :: PBTYPE_32BIT );
 
	ret |= AddString( (char *)&adValue, sizeof( adValue ) );

	return ret;
}

int SKPBEncoder :: AddDouble( const int &aiTag, const double &adValue)
{

	int ret = AddKey( aiTag, SKPBHelper :: PBTYPE_64BIT );
 
	ret |= AddString( (char *)&adValue, sizeof( adValue ) );

	return ret;
}

int SKPBEncoder :: AddKey( const int &aiTag, const int &aiType )
{
	unsigned long long iKey = SKPBHelper :: MakeKey( aiTag, aiType );

	return AddValue( iKey );
}


int SKPBEncoder :: AddTagAndLen(const int &aiTag, const int &aiLen )
{
	int ret = AddKey( aiTag, SKPBHelper :: PBTYPE_LENLIMITED );

	ret |= AddValue( aiLen );

	return ret;
}

template < typename T >
int SKPBEncoder :: AddValue( const T &aValue )
{
	T Value = aValue;

	unsigned long long iBitHeaderMask = 0xfe00000000000000LL;
	
	while( 1 )
	{
		if( m_iUsedSize >= m_iSize )
			return -1;

		char tmp = Value & 0x7f;
	
		if( Value >> 0x7 ) 
		{
			tmp |= 0x80;
		}

		Value >>= 0x7;
		Value &= ~iBitHeaderMask;

		memcpy( m_pcBuffer+m_iUsedSize, &tmp, sizeof(tmp) );
		m_iUsedSize += sizeof( tmp );

		if( !Value ) break;
	}

	if( m_iUsedSize > m_iSize )
		return -1;

	return 0;
}

int SKPBEncoder :: AddString( const char * apcBuf, const int &aiLen )
{
	memcpy( m_pcBuffer+m_iUsedSize, apcBuf, aiLen );
	m_iUsedSize += aiLen;

	return m_iUsedSize > m_iSize;
}

//*********************add array method ********************
int SKPBEncoder :: AddArray( const int &aiTag, const char *apcArray, const int &aiLen,const int &iFroceStore )
{
	if( !aiLen && !iFroceStore )
		return 0;

	int ret = AddKey( aiTag, SKPBHelper :: PBTYPE_LENLIMITED );

	ret |= AddValue( (long long)aiLen );
	
	ret |= AddString( apcArray, aiLen );

	return ret;
}

int SKPBEncoder :: AddArray( const int &aiTag, const unsigned char*apcArray, const int &aiLen,const int &iFroceStore )
{

	if( !aiLen && !iFroceStore )
		return 0;

	int ret = AddKey( aiTag, SKPBHelper :: PBTYPE_LENLIMITED );

	ret |= AddValue( (long long)aiLen );

	ret |= AddString( (const char *)apcArray, aiLen );

	return ret;
}

int SKPBEncoder :: AddArray( const int &aiTag, const short *aphArray, const int &aiLen,const int &iFroceStore )
{

	if( !aiLen && !iFroceStore )
		return 0;

	int ret = AddKey( aiTag, SKPBHelper :: PBTYPE_LENLIMITED );
	
	int iLen = 0;
	for( int i = 0; i < aiLen; ++i )
	{
		iLen += SKPBHelper :: GetVarintLen( (long long)aphArray[i] );
	}
	
	ret |= AddValue( (long long)iLen );

	for( int i = 0; i < aiLen; ++i )
	{
		ret |= AddValue( (long long)aphArray[i] );
	}

	return ret;
}

int SKPBEncoder :: AddArray( const int &aiTag, const unsigned short*aphArray, const int &aiLen,const int &iFroceStore )
{
	if( !aiLen && !iFroceStore )
		return 0;

	int ret = AddKey( aiTag, SKPBHelper :: PBTYPE_LENLIMITED );

	int iLen = 0;
	for( int i = 0; i < aiLen; ++i )
	{
		iLen += SKPBHelper :: GetVarintLen( (unsigned long long)aphArray[i] );
	}

	ret |= AddValue( (long long)iLen );

	for( int i = 0; i < aiLen; ++i )
	{
		ret |= AddValue( (unsigned long long)aphArray[i] );
	}
	return ret;
}

int SKPBEncoder :: AddArray( const int &aiTag, const int *apiArray, const int &aiLen,const int &iFroceStore )
{

	if( !aiLen && !iFroceStore )
		return 0;
	
	int ret = AddKey( aiTag, SKPBHelper :: PBTYPE_LENLIMITED );

	int iLen = 0;
	for( int i = 0; i < aiLen; ++i )
	{
		iLen += SKPBHelper :: GetVarintLen( (long long)apiArray[i] );
	}

	ret |= AddValue( (long long)iLen );

	for( int i = 0; i < aiLen; ++i )
	{
		ret |= AddValue( (long long)apiArray[i] );
	}
	return ret;
}

int SKPBEncoder :: AddArray( const int &aiTag, const unsigned int *apiArray, const int &aiLen,const int &iFroceStore )
{
	if( !aiLen && !iFroceStore )
		return 0;

	int ret = AddKey( aiTag, SKPBHelper :: PBTYPE_LENLIMITED );

	int iLen = 0;
	for( int i = 0; i < aiLen; ++i )
	{
		iLen += SKPBHelper :: GetVarintLen( (unsigned long long)apiArray[i] );
	}
	
	ret |= AddValue( (long long)iLen );

	for( int i = 0; i < aiLen; ++i )
	{
		ret |= AddValue( (unsigned long long)apiArray[i] );
	}
	return ret;
}

int SKPBEncoder :: AddArray( const int &aiTag, const long long *aplArray, const int &aiLen,const int &iFroceStore )
{
	if( !aiLen && !iFroceStore )
		return 0;

	int ret = AddKey( aiTag, SKPBHelper :: PBTYPE_LENLIMITED );

	int iLen = 0;
	for( int i = 0; i < aiLen; ++i )
	{
		iLen += SKPBHelper :: GetVarintLen( (long long)aplArray[i] );
	}
	
	ret |= AddValue( (long long)iLen );

	for( int i = 0; i < aiLen; ++i )
	{
		ret |= AddValue( aplArray[i] );
	}
	return ret;
}

int SKPBEncoder :: AddArray( const int &aiTag, const unsigned long long *aplArray, const int &aiLen,const int &iFroceStore )
{
	if( !aiLen && !iFroceStore )
		return 0;

	int ret = AddKey( aiTag, SKPBHelper :: PBTYPE_LENLIMITED );

	int iLen = 0;
	for( int i = 0; i < aiLen; ++i )
	{
		iLen += SKPBHelper :: GetVarintLen( (unsigned long long)aplArray[i] );
	}
	
	ret |= AddValue( (long long)iLen );

	for( int i = 0; i < aiLen; ++i )
	{
		ret |= AddValue( aplArray[i] );
	}
	return ret;
}

int SKPBEncoder :: AddArray( const int &aiTag, const float *apfArray, const int &aiLen,const int &iFroceStore )
{
	if( !aiLen && !iFroceStore )
		return 0;

	int ret = AddKey( aiTag, SKPBHelper :: PBTYPE_LENLIMITED );

	ret |= AddValue( (long long)( sizeof(float) * aiLen ) );

	for( int i = 0; i < aiLen; ++i )
	{
		ret |= AddString( (char *)(apfArray+i), sizeof( float ) );
	}
	return ret;
}

int SKPBEncoder :: AddArray( const int &aiTag, const double *apdArray, const int &aiLen,const int &iFroceStore )
{
	if( !aiLen && !iFroceStore )
		return 0;

	int ret = AddKey( aiTag, SKPBHelper :: PBTYPE_LENLIMITED );
	
	ret |= AddValue( (long long)( sizeof(double) * aiLen ) );

	for( int i = 0; i < aiLen; ++i )
	{
		ret |= AddString( ( char *)(apdArray+i), sizeof( double ) );
	}
	return ret;
}

int SKPBEncoder :: AddSInt32Array( const int &aiTag, const int *apiArray, const int &aiLen,const int &iFroceStore )
{
	if( !aiLen && !iFroceStore )
		return 0;

	int ret = AddKey( aiTag, SKPBHelper :: PBTYPE_LENLIMITED );

	int iLen = 0;
	for( int i = 0; i < aiLen; ++i )
	{
		iLen += SKPBHelper :: GetVarintLen( SKPBHelper :: Encode32( apiArray[i] ) );
	}
	
	ret |= AddValue( (long long)iLen );

	for( int i = 0; i < aiLen; ++i )
	{
		ret |= AddValue( SKPBHelper :: Encode32( apiArray[i] ) );
	}
	return ret;
}

int SKPBEncoder :: AddSInt64Array( const int &aiTag, const long long *aplArray, const int &aiLen,const int &iFroceStore )
{
	if( !aiLen && !iFroceStore )
		return 0;

	int ret = AddKey( aiTag, SKPBHelper :: PBTYPE_LENLIMITED );

	int iLen = 0;
	for( int i = 0; i < aiLen; ++i )
	{
		iLen += SKPBHelper :: GetVarintLen( SKPBHelper :: Encode64( aplArray[i] ) );
	}
	
	ret |= AddValue( (long long)iLen );

	for( int i = 0; i < aiLen; ++i )
	{
		ret |= AddValue( SKPBHelper :: Encode64( aplArray[i] ) );
	}
	return ret;
}

}

