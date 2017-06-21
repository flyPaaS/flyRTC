
#pragma once

namespace Comm
{

class SKPBHelper
{

public:
	static const int TYPEBIT = 3;
	static const int TYPEMASK = 0x7;

	enum
	{
		PBTYPE_VARINTTYPE=0,
		PBTYPE_64BIT=1,
		PBTYPE_LENLIMITED=2,
		PBTYPE_STARTGROUP=3,
		PBTYPE_ENDGROUP=4,
		PBTYPE_32BIT=5
	};

	static unsigned long long MakeKey(const int &aiTag, const int &aiType);

	static unsigned int Encode32( const int &aiValue );
	static unsigned long long Encode64( const long long &alValue );
	static int Decode32( const unsigned int &aiValue );
	static long long Decode64( const unsigned long long &aiValue );

	static int GetTagAndLenSize( const int &aiTag, const int &aiLen );

	static int GetSize( const int &aiTag, const long long &alValue );
	static int GetSize( const int &aiTag, const unsigned long long &alValue );
	static int GetSize( const int &aiTag, const float &afValue );
	static int GetSize( const int &aiTag, const double &adValue );

	static int GetSInt32Size( const int &aiTag, const int &alValue );
	static int GetSInt64Size( const int &aiTag, const long long &alValue );


	static int GetArraySize( const int &aiTag, const char * apiArray,
			const int &aiLen, const int &iForceStore = 1 );
	static int GetArraySize( const int &aiTag, const unsigned char* apiArray,
			const int &aiLen, const int &iForceStore = 1 );
	static int GetArraySize( const int &aiTag, const short* apiArray,
			const int &aiLen, const int &iForceStore = 1 );
	static int GetArraySize( const int &aiTag, const unsigned short* apiArray,
			const int &aiLen, const int &iForceStore = 1 );
	static int GetArraySize( const int &aiTag, const int * apiArray,
			const int &aiLen, const int &iForceStore = 1 );
	static int GetArraySize( const int &aiTag, const unsigned int* apiArray,
			const int &aiLen, const int &iForceStore = 1 );
	static int GetArraySize( const int &aiTag, const long long* aplArray,
			const int &aiLen, const int &iForceStore = 1 );
	static int GetArraySize( const int &aiTag, const unsigned long long* aplArray,
			const int &aiLen, const int &iForceStore = 1 );
	static int GetArraySize( const int &aiTag, const float* apfArray,
			const int &aiLen, const int &iForceStore = 1 );
	static int GetArraySize( const int &aiTag, const double* apdArray,
			const int &aiLen, const int &iForceStore = 1 );

	static int GetSInt32ArraySize( const int &aiTag, const int * aplArray,
			const int &aiLen, const int &iForceStore = 1 );
	static int GetSInt64ArraySize( const int &aiTag, const long long* aplArray,
			const int &aiLen, const int &iForceStore = 1 );

	template < typename T > 
		static int GetVarintLen( const T &aValue )
		{
			unsigned long long iBitHeaderMask = 0xfe00000000000000LL;

			int iLen = 0;
			T Value = aValue;

			while( 1 )
			{
				++iLen;
				Value >>= 0x7;
				Value &= ~iBitHeaderMask;
				if( !Value ) 
				{
					break;
				}
			}
			return iLen;
		}

private:
	template < typename T >   static int GetValueWithTagSize( const int &aiTag, const T &aValue );

};

}

