#include "iMsgBodyXP.h"
#include <string.h>

namespace Comm
{

CMsgBodyXP::CMsgBodyXP() :
	CTLVPack(0)
{
}

CMsgBodyXP::~CMsgBodyXP()
{
}

int CMsgBodyXP::InitFromBuffer(const char *apBuf, int aiSize)
{
	if (aiSize < 0 || (aiSize != 0 && aiSize < 2))
	{
		return NMXPRC_ERR_INVALID_ARG;
	}

	if (apBuf == NULL || aiSize == 0)
	{
		Clear(); // clear exist items
		return NMXPRC_OK;
	}

	// check pkg format
	unsigned char *pPkg = (unsigned char *)apBuf;
	if (pPkg[0] != (unsigned char)0xBE && 
		pPkg[aiSize-1] != (unsigned char)0xED)
	{
		return NMXPRC_ERR_INVALID_ARG;
	}	

	if (2 == aiSize)
	{
		/* only verify bytes */
		Clear(); // clear exist items
		return NMXPRC_OK;
	}

	if (0 > CopyFrom(apBuf + 1, aiSize - 2))
	{
		return NMXPRC_ERR_GENERAL;
	}

	return NMXPRC_OK;
}

int CMsgBodyXP::PackToBuffer(char **appBuf, int *apiSize)
{
	if( appBuf == NULL || apiSize == NULL )
		return NMXPRC_ERR_INVALID_ARG;

	*appBuf = NULL;
	memset((char*)apiSize, 0, sizeof(int));

	int iTLVSize = GetUsedSize();

	int iTotalSize = iTLVSize + 2; // 2 byte header and footer

	*appBuf = new char[iTotalSize]; 
	if (*appBuf == NULL)
	{
		return NMXPRC_ERR_NOMEM;
	}

	char *pcBegin = *appBuf;
	memset(pcBegin, 0, iTotalSize);
	memcpy(apiSize, &iTotalSize, sizeof(int));

	*pcBegin = (char)0xBE; // begin char
	pcBegin += 1;
	int iRetCode = CopyTo(pcBegin, &iTLVSize);
	if (iRetCode < 0)
	{
		delete[](*appBuf);
		*appBuf = NULL;
		return NMXPRC_ERR_GENERAL;
	}
	pcBegin += iTLVSize;
	*pcBegin = (char)0xED; // end char

	return NMXPRC_OK;
}

}

