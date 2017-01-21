
#include "skprotohelper.h"
#include "skmetainfo.h"
#include "iSKTLVBuffer.h"

#define TLVMAGIC 0x81
namespace Comm
{

	inline void SKProtoHelper :: ToLittleEndian(void *x, int len)
	{
		if (len < 2)
		{
			return;
		}
		
			int i = 4660;
			char c = 0;
			if (((char*)&i)[0] != 0x34)
			{
				char *p = (char *)x;
					for (i = 0; i < len/2; ++i)
					{
						c = p[i];
							p[i] = p[len-i-1];
							p[len-i-1] = c;
					}
			}
	}
	
	unsigned short SKProtoHelper :: GetCheckSum( const char * pcBuf, const int &iBufLen )
	{
		unsigned short uCheckSum = 0;
		for(int i=0;i<iBufLen;++i)
		{
			uCheckSum += ( unsigned short )( signed char )pcBuf[i];
		}

		uCheckSum = ~uCheckSum;
		ToLittleEndian(&uCheckSum, sizeof(unsigned short));

		return uCheckSum;
	}

	int SKProtoHelper :: JudgeProtoType( const char * pcBuf, const int &iBufLen )
	{
		if( iBufLen < (int)sizeof( tagTLVPackHeader ) )
		{
			return ePickleTypeProtoBuf;
		}
		else
		{	
			tagTLVPackHeader * pcPtr =  ( tagTLVPackHeader * ) pcBuf;

			if( pcPtr -> uSize != iBufLen - sizeof( tagTLVPackHeader ) 
				|| pcPtr-> cMagic != TLVMAGIC
				|| pcPtr -> cMode > 1
				|| pcPtr -> uReserved != 0 )
			{
				return ePickleTypeProtoBuf;
			}

			unsigned short uCheckSum = ( ( tagTLVPackHeader * ) pcBuf ) -> wCheckSum;
			if( GetCheckSum( pcBuf + sizeof( tagTLVPackHeader ), iBufLen - sizeof( tagTLVPackHeader  ) ) == uCheckSum )
				return ePickleTypeTlv;
			else
				return ePickleTypeProtoBuf;
		}
		return -1;
	}
}
