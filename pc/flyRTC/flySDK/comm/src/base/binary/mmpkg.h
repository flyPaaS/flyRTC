#pragma once 

#include "iTlvMsgXP.h"

typedef struct tagBodyHead_t 
{
	int Ret;
	unsigned int Uin;
	unsigned short CmdId;
	int ServerIdLen;
	unsigned char * ServerId;
	unsigned char DeviceId[16];
	short CompressVersion;
	short CompressAlgorithm;
	short CryptAlgorithm;
	unsigned int CompressLen;
	unsigned int CompressedLen;
}BodyHead_t;

class MMPkg
{
	public:
		MMPkg();

		~MMPkg();
		
		BodyHead_t * GetHead();

		Comm::SKBuffer * GetBody();

		int Pack( Comm::SKBuffer * poBuffer );

		int Unpack( Comm::SKBuffer * poBuffer );

	private:
		
		BodyHead_t * m_poHead;
		Comm::SKBuffer * m_poBody;
};
