#pragma once

#include "iTypeXP.h"

namespace Comm
{

// Minimum magic value
#define QQMAIL_APP_MAGIC_MIN_XP 0x0200

// deprecated, as all numerical value are stored in NetOrder. but this 
// field is reserved for compatible purpose
#define QQMAIL_BYTEORDER_XP 1234

enum emNetMsgXPRetCode
{
	NMXPRC_OK = 0,

	NMXPRC_ERR_GENERAL = -1,
	NMXPRC_ERR_NOMEM = -2,
	NMXPRC_ERR_INVALID_ARG = -3,
	NMXPRC_ERR_DATA = -4,
};

	struct tMsgHeadXp ;
	typedef tMsgHeadXp QQMAILMSGHEADXP ;
	typedef tMsgHeadXp *LPQQMAILMSGHEADXP ;

#pragma pack(1)

	/* 32 bytes */
	struct tMsgHeadXp
	{
		unsigned short hMagic;
		unsigned char bVersion;
		unsigned char bHeadLen;
		unsigned int iBodyLen;
		unsigned short hCmdID;
		unsigned short hHeadChkSum;
		unsigned int iSequence;
		unsigned int iByteOrder; ///< this value will always be value 1234 in netorder
		unsigned int iUin;
		unsigned short hResult; /* if hResult <> 0, the first field of body is detail ErrMsg buffer */
		unsigned char sReserved[6];
	} ;

#pragma pack()

}

