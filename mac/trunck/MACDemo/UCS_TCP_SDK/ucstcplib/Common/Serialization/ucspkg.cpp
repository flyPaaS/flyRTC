#include "ucspkg.h"
#include "ucscompress.h"
#include "xyssl.h"

using Comm::SKBuffer;
using Comm::CMsgBodyXP;
using Comm::CTLVPack;

UCSPkg:: UCSPkg()
{
	m_poHead = new BodyHead_t;
	memset( m_poHead, 0, sizeof( BodyHead_t ));
	m_poBody = new SKBuffer;
}

UCSPkg:: ~UCSPkg()
{
	if( m_poBody )
	{
		delete m_poBody;
	}
	if( m_poHead->ServerId )
	{
		delete [] m_poHead->ServerId;
	}
	if( m_poHead )
	{
		delete m_poHead;
	}
}

BodyHead_t* UCSPkg:: GetHead()
{
	return m_poHead;
}

SKBuffer* UCSPkg:: GetBody()
{
	return m_poBody;
}

int UCSPkg:: Pack( SKBuffer * poBuffer )
{
	int ret = 0;
	
	CMsgBodyXP body;
	body.SetMode(CTLVPack::TLV_VARIABLE_TL_SIZE);
	body.AddChar( 1, 0xBE );
	body.AddUInt( 2, m_poHead->Uin );
	body.AddWord( 3, m_poHead->CmdId );
	body.AddInt( 4, m_poHead->ServerIdLen );
	body.AddBuf( 5, (char *)m_poHead->ServerId, m_poHead->ServerIdLen );
	body.AddBuf( 6, (char *)m_poHead->DeviceId, 16 );
	body.AddShort( 7, m_poHead->CompressVersion );
	body.AddShort( 8, m_poHead->CompressAlgorithm );
	body.AddShort( 9, m_poHead->CryptAlgorithm );
	body.AddUInt( 10, m_poHead->CompressLen );
	body.AddUInt( 11, m_poHead->CompressedLen );
	body.AddBuf( 12, (char *)m_poBody->GetBuffer(), m_poBody->GetLen() );
	body.AddInt( 13, m_poHead->Ret );
	body.AddChar(14, 0xED );
    //body.AddUInt(14, m_poHead->CertVersion);
	//body.AddChar(15, 0xED );

	char * tempBuf = NULL;
	int tempSize = 0;
	ret = body.PackToBuffer( &tempBuf, &tempSize );
	
	if( ret != 0 )
	{
		return ret;
	}

	poBuffer->Write( tempBuf, tempSize );
	
	delete []tempBuf;
	return ret;
}
	
int UCSPkg:: Unpack( SKBuffer * poBuffer )
{
	int ret = 0;

	CMsgBodyXP body;
	
	ret = body.InitFromBuffer( (char*)poBuffer->GetBuffer(), poBuffer->GetLen() );
	
	if( ret != 0 )
	{
		return -1;
	}

	body.GetUInt( 2, &m_poHead->Uin );
	body.GetWord( 3, &m_poHead->CmdId );
	body.GetInt( 4, &m_poHead->ServerIdLen );
	m_poHead->ServerId = new unsigned char[m_poHead->ServerIdLen];
	body.GetBuf( 5, (char *)m_poHead->ServerId, &m_poHead->ServerIdLen );
	int len = sizeof( m_poHead->DeviceId );
	body.GetBuf( 6, (char *)m_poHead->DeviceId, &len );
	body.GetShort( 7, &m_poHead->CompressVersion );
	body.GetShort( 8, &m_poHead->CompressAlgorithm );
	body.GetShort( 9, &m_poHead->CryptAlgorithm );
	body.GetUInt( 10, &m_poHead->CompressLen );
	body.GetUInt( 11, &m_poHead->CompressedLen );

	len = 0;
	body.GetBuf( 12, NULL, &len );

	char * ptr = new char[ len + 1 ];
	body.GetBuf( 12, ptr, &len );

	m_poBody->Write( ptr, len );
	delete []ptr;
	
	body.GetInt( 13, &m_poHead->Ret );
    //body.GetUInt(14, m_poHead->CertVersion);
	
	return ret;
}


