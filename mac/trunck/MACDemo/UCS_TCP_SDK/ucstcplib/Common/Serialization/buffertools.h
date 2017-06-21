#pragma once

namespace Comm
{
	class SKBuffer;
}
class BufferTools
{
	public:
	static int Buffer2String( const unsigned char * pcBuffer, const int &iLen, char ** pcStr );
	static int String2Buffer( const char * pcStr, unsigned char ** pcBuffer, int * iLen );

	static int Buffer2String( const Comm::SKBuffer * poBuffer, char ** pcStr );
	static int String2Buffer( const char * pcStr, Comm::SKBuffer *poBuffer );
	private:
	static int Encode(unsigned char val, char ** pcStr);
	static int Decode(const char ** pcStr, unsigned char &val);
};
