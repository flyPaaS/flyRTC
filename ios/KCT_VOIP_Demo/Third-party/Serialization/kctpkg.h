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
    //unsigned int CertVersion;
}BodyHead_t;

class KCTPkg
{
	public:
		KCTPkg();

		~KCTPkg();
		
		BodyHead_t * GetHead();

		Comm::SKBuffer * GetBody();

		int Pack( Comm::SKBuffer * poBuffer );

		int Unpack( Comm::SKBuffer * poBuffer );

	private:
		
		BodyHead_t * m_poHead;
		Comm::SKBuffer * m_poBody;
};
