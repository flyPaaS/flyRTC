
#include "skpbhelper.h"

namespace Comm
{

unsigned long long SKPBHelper ::  MakeKey( const int &aiTag, const int &aiType )
{
	return (long long )aiTag << TYPEBIT | aiType;
}

unsigned int SKPBHelper :: Encode32 ( const int &aiValue )
{
	return ( aiValue << 1 ) ^ ( aiValue >> 31 ) ;
}

unsigned long long SKPBHelper :: Encode64 ( const long long &alValue )
{
	return ( alValue << 1 ) ^ ( alValue >> 63 ) ;
}

int SKPBHelper :: Decode32( const unsigned int &aiValue )
{
	unsigned int uValue = (unsigned int) aiValue;
	return ( uValue >> 1 ) ^ ( (int)(uValue & 0x1) << 31 >> 31 );
}

long long SKPBHelper :: Decode64( const unsigned long long &alValue )
{
	unsigned long long uValue = (unsigned long long) alValue;
	return ( uValue >> 1 ) ^ ( (long long )(uValue & 0x1) << 63 >> 63 );
}


template <typename T>
int SKPBHelper :: GetValueWithTagSize( const int &aiTag, const T &aValue )
{
	return GetVarintLen( aValue ) + GetVarintLen ( MakeKey( aiTag, PBTYPE_VARINTTYPE ) );
}


int SKPBHelper :: GetSize( const int &aiTag, const long long &alValue )
{
	return GetValueWithTagSize( aiTag, alValue );
}

int SKPBHelper :: GetSize( const int &aiTag, const unsigned long long &alValue )
{
	return GetValueWithTagSize( aiTag, alValue );
}

int SKPBHelper :: GetSize( const int &aiTag, const float &afValue )
{
	unsigned long long lKey = MakeKey( aiTag, PBTYPE_32BIT );
	return sizeof( afValue ) + GetVarintLen ( lKey );
}

int SKPBHelper :: GetSize( const int &aiTag, const double &adValue )
{
	unsigned long long lKey = MakeKey( aiTag, PBTYPE_64BIT );
	return sizeof( adValue ) + GetVarintLen( lKey );
}

int SKPBHelper :: GetSInt32Size( const int &aiTag, const int &aiValue )
{
	return GetValueWithTagSize( aiTag, Encode32( aiValue ) );
}

int SKPBHelper :: GetSInt64Size( const int &aiTag, const long long &alValue )
{
	return GetValueWithTagSize( aiTag, Encode64( alValue ) );
}

int SKPBHelper :: GetTagAndLenSize( const int &aiTag, const int &aiLen )
{
	unsigned long long lKey = MakeKey( aiTag, PBTYPE_LENLIMITED );

	return  GetVarintLen( lKey ) + GetVarintLen( aiLen );
}
//**************************************************************

int SKPBHelper ::  GetArraySize( const int &aiTag, const char * apcArray, const int &aiLen, const int &iForceStore )
{
	if( !aiLen && !iForceStore )
		return 0;

	unsigned long long lKey = MakeKey( aiTag, PBTYPE_LENLIMITED );
	int len = aiLen;
	len += GetVarintLen( lKey ) + GetVarintLen( (long long) aiLen );
	return len;

}

int SKPBHelper ::  GetArraySize( const int &aiTag, const unsigned char* apcArray, const int &aiLen, const int &iForceStore )
{

	if( !aiLen && !iForceStore )
		return 0;

	unsigned long long lKey = MakeKey( aiTag, PBTYPE_LENLIMITED );
	int len = aiLen;
	len += GetVarintLen( lKey ) + GetVarintLen( (long long) aiLen );
	return len;

}

int SKPBHelper ::  GetArraySize( const int &aiTag, const short* aphArray, const int &aiLen, const int &iForceStore )
{

	if( !aiLen && !iForceStore )
		return 0;

	unsigned long long lKey = MakeKey( aiTag, PBTYPE_LENLIMITED );
	int len = 0;
	for( int i = 0; i < aiLen; ++i )
	{
		len += GetVarintLen(  ( long long )aphArray[i]  );
	}
	len += GetVarintLen( lKey ) + GetVarintLen( (long long)len );
	return len;

}

int SKPBHelper ::  GetArraySize( const int &aiTag, const unsigned short * aphArray, const int &aiLen, const int &iForceStore )
{

	if( !aiLen && !iForceStore )
		return 0;


	unsigned long long lKey = MakeKey( aiTag, PBTYPE_LENLIMITED );
	int len = 0;
	for( int i = 0; i < aiLen; ++i )
	{
		len += GetVarintLen(  ( unsigned long long )aphArray[i]  );
	}
	len += GetVarintLen( lKey ) + GetVarintLen( (long long)len );
	return len;

}

int SKPBHelper ::  GetArraySize( const int &aiTag, const int * apiArray, const int &aiLen, const int &iForceStore )
{

	if( !aiLen && !iForceStore )
		return 0;


	unsigned long long lKey = MakeKey( aiTag, PBTYPE_LENLIMITED );
	int len = 0;
	for( int i = 0; i < aiLen; ++i )
	{
		len += GetVarintLen(  ( long long )apiArray[i]  );
	}
	len += GetVarintLen( lKey ) + GetVarintLen( (long long)len );
	return len;

}

int SKPBHelper ::  GetArraySize( const int &aiTag, const unsigned int* apiArray, const int &aiLen, const int &iForceStore )
{

	if( !aiLen && !iForceStore )
		return 0;


	unsigned long long lKey = MakeKey( aiTag, PBTYPE_LENLIMITED );
	int len = 0;
	for( int i = 0; i < aiLen; ++i )
	{
		len += GetVarintLen(  ( unsigned long long )apiArray[i]  );
	}
	len += GetVarintLen( lKey ) + GetVarintLen( (long long)len );
	return len;

}

int SKPBHelper ::  GetArraySize( const int &aiTag, const long long* aplArray, const int &aiLen, const int &iForceStore )
{

	if( !aiLen && !iForceStore )
		return 0;


	unsigned long long lKey = MakeKey( aiTag, PBTYPE_LENLIMITED );
	int len = 0;
	for( int i = 0; i < aiLen; ++i )
	{
		len += GetVarintLen(  aplArray[i]  );
	}
	len += GetVarintLen( lKey ) + GetVarintLen( (long long)len );
	return len;

}

int SKPBHelper ::  GetArraySize( const int &aiTag, const unsigned long long* aplArray, const int &aiLen, const int &iForceStore )
{

	if( !aiLen && !iForceStore )
		return 0;


	unsigned long long lKey = MakeKey( aiTag, PBTYPE_LENLIMITED );
	int len = 0;
	for( int i = 0; i < aiLen; ++i )
	{
		len += GetVarintLen(  aplArray[i]  );
	}
	len += GetVarintLen( lKey ) + GetVarintLen( (long long)len );
	return len;
}

int SKPBHelper ::  GetArraySize( const int &aiTag, const float* apfArray, const int &aiLen, const int &iForceStore )
{

	if( !aiLen && !iForceStore )
		return 0;

	unsigned long long lKey = MakeKey( aiTag, PBTYPE_LENLIMITED );
	int len = 0;
	
	len = sizeof( float ) * aiLen;

	len += GetVarintLen( lKey ) + GetVarintLen( (long long)len );
	return len;
}
int SKPBHelper ::  GetArraySize( const int &aiTag, const double* apdArray, const int &aiLen, const int &iForceStore )
{

	if( !aiLen && !iForceStore )
		return 0;

	unsigned long long lKey = MakeKey( aiTag, PBTYPE_LENLIMITED );
	int len = 0;
	
	len = sizeof( double ) * aiLen;
	len += GetVarintLen( lKey ) + GetVarintLen( (long long)len );
	return len;
}

int SKPBHelper :: GetSInt32ArraySize( const int &aiTag, const int * apiArray, const int &aiLen, const int &iForceStore )
{

	if( !aiLen && !iForceStore )
		return 0;

	unsigned long long lKey = MakeKey( aiTag, PBTYPE_LENLIMITED );
	int len = 0;
	for( int i = 0; i < aiLen; ++i )
	{
		len += GetVarintLen(  Encode32( apiArray[i] ) );
	}

	len += GetVarintLen( lKey ) + GetVarintLen( (long long)len );
	return len;
}

int SKPBHelper :: GetSInt64ArraySize( const int &aiTag, const long long* aplArray, const int &aiLen, const int &iForceStore )
{

	if( !aiLen && !iForceStore )
		return 0;

	unsigned long long lKey = MakeKey( aiTag, PBTYPE_LENLIMITED );
	int len = 0;
	for( int i = 0; i < aiLen; ++i )
	{
		len += GetVarintLen(  Encode64( aplArray[i] ) );
	}

	len += GetVarintLen( lKey ) + GetVarintLen( (long long)len );
	return len;
}

}

