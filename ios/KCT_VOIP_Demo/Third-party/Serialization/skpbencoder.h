#pragma once

namespace Comm
{

class SKPBEncoder
{
public:
	SKPBEncoder( const int &aiSize );
	~SKPBEncoder();
public:
	int Detach( char ** apcBuf, int * aiLen );

public:
	int AddInt64( const int &aiTag, const long long &alValue);
	int AddUInt64( const int &aiTag, const unsigned long long &alValue);
	int AddFloat( const int &aiTag, const float &afValue);
	int AddDouble( const int &aiTag, const double &adValue);

	int AddSInt32( const int &aiTag, const int &alValue);
	int AddSInt64( const int &aiTag, const long long &alValue);
	
	int AddArray( const int &aiTag, const char *apiArray, const int &aiLen, const int &iForceStore = 1 );
	int AddArray( const int &aiTag, const unsigned char *apiArray, const int &aiLen, const int &iForceStore = 1 );
	int AddArray( const int &aiTag, const short *apiArray, const int &aiLen,const int &iForceStore = 1 );
	int AddArray( const int &aiTag, const unsigned short *apiArray, const int &aiLen,const int &iForceStore = 1 );
	int AddArray( const int &aiTag, const int *apiArray, const int &aiLen,const int &iForceStore = 1 );
	int AddArray( const int &aiTag, const unsigned int *apiArray, const int &aiLen,const int &iForceStore = 1 );
	int AddArray( const int &aiTag, const long long *aplArray, const int &aiLen,const int &iForceStore = 1 );
	int AddArray( const int &aiTag, const unsigned long long *aplArray, const int &aiLen,const int &iForceStore = 1 );
	int AddArray( const int &aiTag, const float *apfArray, const int &aiLen,const int &iForceStore = 1 );
	int AddArray( const int &aiTag, const double *apdArray, const int &aiLen,const int &iForceStore = 1 );

	int AddSInt32Array( const int &aiTag, const int *apdArray, const int &aiLen,const int &iForceStore = 1 );
	int AddSInt64Array( const int &aiTag, const long long *apdArray, const int &aiLen,const int &iForceStore = 1 );

	int AddTagAndLen(const int &aiTag, const int &aiLen ); // for lenlimited

private:
	int AddString( const char * apcBuc, const int &aiLen );
	int AddKey( const int &aiTag, const int &aiType );

	template < typename T > int AddValue( const int &aiTag, const T &aValue );
	template < typename T > int AddValue( const T &aValue );
private:
	char *m_pcBuffer;
	int m_iSize;
	int m_iUsedSize;

};

}
