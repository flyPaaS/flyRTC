
#pragma once

namespace Comm
{

enum
{
	SKPB_EMPTYSTRUCT = -2
};

class SKPBDecoder
{
public:
	SKPBDecoder ( );
	~SKPBDecoder();
public:
	int Attach( char * apcBuf, const int &aiSize );
	int Detach( );
	int GetNextObject();
	int GetSubObject( SKPBDecoder * apoBuffer );
	int GetTag();
	int GetType();
	int Reset();


public:
	int GetArray( char *apiArray, const int &aiLen );
	int GetArray( unsigned char *apiArray, const int &aiLen );
	int GetArray( short *apiArray, const int &aiLen );
	int GetArray( unsigned short *apiArray, const int &aiLen );
	int GetArray( int *apiArray, const int &aiLen );
	int GetArray( unsigned int *apiArray, const int &aiLen );
	int GetArray( long long *aplArray, const int &aiLen );
	int GetArray( unsigned long long *aplArray, const int &aiLen );
	int GetArray( float *apfArray, const int &aiLen );
	int GetArray( double *apdArray, const int &aiLen );

	int GetSInt32Array( int *apiArray, const int &aiLen );
	int GetSInt64Array( long long *aplArray, const int &aiLen );

	int GetInt64( long long &alValue );
	int GetUInt64( unsigned long long &aiValue );
	int GetFloat( float &afValue );
	int GetDouble( double &adValue );

	int GetSInt32( int &aiValue );
	int GetSInt64( long long &alValue );

	char * GetString( const int &aiLen );

	int GetStringArrayLen( const int &aiTag, int &aiLen );
private:
	int GetTextLenWithLenField();
	int GetTextLenWithOutLenField();
	int InitObject( char * apcBuf, const int &aiSize );

	template < typename T > int GetArrayValue( T *aValue, const int &iSize );
	template < typename T > int GetValue( T &aValue );
private:
	char *m_pcBuffer;
	char *m_pcOrigHeader;
	int m_iOrigSize;
	int m_iSize;
	int m_iType;
	int m_iTag;
	int m_iUnReadPos;
	int m_iVarintLen;
	int m_iKeySize;

};

}

