#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#include "iMsgHeadXP.h"

namespace Comm
{

CMsgHeadXP::CMsgHeadXP() 
{
	Clear();
}


CMsgHeadXP::~CMsgHeadXP()
{
	Clear();
}


void CMsgHeadXP::Clear()
{
	memset((char*)&m_tMsgHead, 0x0, sizeof(m_tMsgHead)) ;
	memset((char*)m_sBuffer, 0x0, sizeof(m_sBuffer)) ;
}


/* ********* SetXXX() function set begin ********* */
void CMsgHeadXP :: SetMagic ( const unsigned short a_hMagic )
{
	m_tMsgHead.hMagic = htons(a_hMagic) ;
	return ;
}


void CMsgHeadXP :: SetVersion ( const unsigned char a_bVersion )
{
	m_tMsgHead.bVersion = a_bVersion ;
	return ;
}


void CMsgHeadXP :: SetCmdID ( const unsigned short a_hCmdID )
{
	m_tMsgHead.hCmdID = htons(a_hCmdID) ;
	return ;
}


void CMsgHeadXP :: SetBodyLen ( const unsigned int a_iBodyLen )
{
	m_tMsgHead.iBodyLen = htonl(a_iBodyLen) ;
	return ;
}


void CMsgHeadXP :: SetSequence ( const unsigned int a_iSeq )
{
	m_tMsgHead.iSequence = htonl(a_iSeq) ;
	return ;
}


void CMsgHeadXP :: SetUin ( const unsigned int a_iUin )
{
	m_tMsgHead.iUin = htonl(a_iUin) ;
	return ;
}


void CMsgHeadXP :: SetResult ( const unsigned short a_hResult )
{
	m_tMsgHead.hResult = htons(a_hResult) ;
	return ;
}


void CMsgHeadXP :: SetReserved ( const unsigned char *a_pcReserved, const int a_iLen )
{
	int iLen = 0 ;

	if ( (NULL == a_pcReserved) || (0 >= a_iLen) )
	{
		memset ( m_tMsgHead.sReserved, 0, sizeof(m_tMsgHead.sReserved) ) ;
	}
	else
	{
		iLen = ( (a_iLen > (int)sizeof(m_tMsgHead.sReserved))? sizeof(m_tMsgHead.sReserved) : a_iLen ) ;
		memcpy ( m_tMsgHead.sReserved, a_pcReserved, iLen ) ;
	}

	return ;
}
/* ********* SetXXX() function set end ********* */


/* ********* GetXXX() function set begin ********* */
int CMsgHeadXP :: GetMagic ( )
{
	return ntohs(m_tMsgHead.hMagic) ;
}

int CMsgHeadXP :: GetVersion ( )
{
	return m_tMsgHead.bVersion ;
}

int CMsgHeadXP :: GetCmdID ( )
{
	return ntohs(m_tMsgHead.hCmdID) ;
}

int CMsgHeadXP :: GetBodyLen ( )
{
	return ntohl(m_tMsgHead.iBodyLen) ;
}

int CMsgHeadXP :: GetHeadLen ( )
{
	return sizeof(Comm::QQMAILMSGHEADXP) ;
}

unsigned int CMsgHeadXP :: GetSequence ( )
{
	return ntohl(m_tMsgHead.iSequence) ;
}

int CMsgHeadXP :: GetByteOrder ( )
{
	return ntohl(m_tMsgHead.iByteOrder) ;
}

unsigned short CMsgHeadXP :: GetResult ( )
{
	return ntohs(m_tMsgHead.hResult) ;
}

unsigned int CMsgHeadXP :: GetUin ( )
{
	return ntohl(m_tMsgHead.iUin) ;
}

/* ********* GetXXX() function set end ********* */


char *CMsgHeadXP :: PackHead ( int *a_piSize )
{
	char *pcHead = NULL ;

	if ( NULL != a_piSize )
	{
		memset((char*)a_piSize, 0, sizeof(int));
	}

	m_tMsgHead.bHeadLen = GetHeadLen() ;
	m_tMsgHead.iByteOrder = htonl(QQMAIL_BYTEORDER_XP) ;

	pcHead = new char[GetHeadLen()] ;
	if ( NULL != pcHead )
	{
		/* reset checksum to 0 first */
		m_tMsgHead.hHeadChkSum = 0 ; 
		m_tMsgHead.hHeadChkSum = htons(CheckSum((unsigned char *)&m_tMsgHead, GetHeadLen()));

		memcpy ( pcHead, &m_tMsgHead, GetHeadLen() ) ;

		if ( NULL != a_piSize )
		{
			int iLen = GetHeadLen();
			memcpy(a_piSize, &iLen, sizeof(int));
		}
	}

	return pcHead ;
}


int CMsgHeadXP :: UnpackHead ( const char *a_pcBuffer )
{
	Comm::QQMAILMSGHEADXP tMsgHead ;
	unsigned short hBufferSum = 0 ;
	unsigned short hCheckSum = 0 ;
	int iLen = 0 ;

	memset ( &tMsgHead, 0x0, sizeof(tMsgHead) ) ;
	iLen = GetHeadLen() ;

	memcpy ( &tMsgHead, a_pcBuffer, sizeof(tMsgHead) ) ;

	/* check checksum */
	{
		hBufferSum = ntohs(tMsgHead.hHeadChkSum) ;
		tMsgHead.hHeadChkSum = 0 ; /* reset to 0 first */

		hCheckSum = CheckSum ( (unsigned char *)&tMsgHead, iLen ) ;
		if ( hBufferSum != hCheckSum )
		{
			iLen = Comm::NMXPRC_ERR_DATA ;
		}
		else
		{
			iLen = Comm::NMXPRC_OK ;
			memcpy ( &m_tMsgHead, &tMsgHead, sizeof(m_tMsgHead) ) ;
			m_tMsgHead.hHeadChkSum = htons(hBufferSum) ; /* restore checksum */
		}
	}

	return iLen ;
}


unsigned short CMsgHeadXP :: CheckSum(const unsigned char *pBuffer, int nBufLen)
{
    assert(pBuffer && nBufLen > 0);

    unsigned long nSum = 0;
    unsigned short *pData = (unsigned short *) pBuffer;

    int nLen = nBufLen / 2;
    int nMod = nBufLen % 2;

    /*
     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits INT32o the lower 16 bits.
     */

    int i = 0 ;
    for (i = 0; i < nLen; i++)
    {
        nSum += pData[i];
    }

    unsigned char nShort = 0;

    /* 4mop up an odd byte, if necessary */
    if (nMod == 1)
    {
        nShort = pBuffer[nBufLen - 1];
        nSum += nShort;
    }

    /* 4add back carry outs from top 16 bits to low 16 bits */
    /* add hi 16 to low 16 */
    nSum = (nSum >> 16) + (nSum & 0xffff);

    /* add carry */
    nSum += (nSum >> 16);

    /* truncate to 16 bits */
    return ~((unsigned short) nSum);
}

}

