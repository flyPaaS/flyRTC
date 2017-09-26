#include "iTLVItem.h"
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "iVByte.h"
#include "tlvport.h"
#include "stdio.h"

namespace Comm
{


const int c_iSizeofType = sizeof(int);		///< Type is a Integer32
const int c_iSizeofLength = sizeof(int);	///< Length is a Integer32

tTLVItem::tTLVItem()
{
	iType = 0;
	iLength = 0;
	pcValPtr = NULL;
	iNextOffset = 0;
}


bool tFixedSizeTLVItem::MapTo(const char *apcSrc, const int aiSrcSize, const int aiOffset)
{
	if (apcSrc == NULL ||
		aiSrcSize <= 0 ||
		aiOffset < 0 ||
		aiOffset >= aiSrcSize)
	{
		return false;
	}

	int iTmp = 0;
	memcpy((char*)&iTmp, (char*)apcSrc + aiOffset, c_iSizeofType);
	int iNewType = ntohl(iTmp);

	memcpy((char*)&iTmp, (char*)apcSrc + aiOffset + c_iSizeofType, c_iSizeofLength);
	int iNewLength = ntohl(iTmp);

	char *pcNewValPtr  = (char*)apcSrc + aiOffset + c_iSizeofType + c_iSizeofLength;
	int iNewNextOffset = aiOffset + c_iSizeofType + c_iSizeofLength + iNewLength;

	if (iNewLength < 0)
	{
		TLV_LOG_ERR ( "TLVITEM: mapto failed newlen %i", iNewLength );
		return false; // Invalid length, package is corrupted
	}

	if (pcNewValPtr > (apcSrc + aiSrcSize))
	{
		TLV_LOG_ERR ( "TLVITEM: mapto failed ptr range 0x%p 0x%p srcsize %i",
			pcNewValPtr, apcSrc, aiSrcSize );
		return false; // Value exceeded the edge, package is corrupted
	}

	if (iNewNextOffset > aiSrcSize)
	{
		TLV_LOG_ERR ( "TLVITEM: mapto failed offset %i srcsize %i",
			iNewNextOffset, aiSrcSize );
		return false; // Value length over the edge, package is corrupted
	}

	if (iNewNextOffset == aiSrcSize)
	{
		iNextOffset = 0; // at the edge
	}
	else
	{
		iNextOffset = iNewNextOffset;
	}		

	iType = iNewType;
	iLength = iNewLength;
	pcValPtr = pcNewValPtr;

	return true;
}

bool tVariableSizeTLVItem::MapTo(const char *apcSrc, const int aiSrcSize, const int aiOffset)
{
	if (apcSrc == NULL ||
		aiSrcSize <= 0 ||
		aiOffset < 0 ||
		aiOffset >= aiSrcSize)
	{
		return false;
	}

	const unsigned char *pcPos = (const unsigned char *)apcSrc + aiOffset;

	int iNewType = 0;
	int iNewTypeLen = DecodeVByte32((unsigned int*)&iNewType, pcPos);
	pcPos += iNewTypeLen;

	int iNewLength = 0;
	int iNewLengthLen = DecodeVByte32((unsigned int*)&iNewLength, pcPos);
	pcPos += iNewLengthLen;

	int iNewNextOffset = aiOffset + iNewTypeLen + iNewLengthLen + iNewLength;

	if (iNewLength < 0)
	{
		return false; // Invalid length, package is corrupted
	}

	if (pcPos > (const unsigned char*)(apcSrc + aiSrcSize))
	{
		return false; // Value exceeded the edge, package is corrupted
	}

	if (iNewNextOffset > aiSrcSize)
	{
		return false; // Value length over the edge, package is corrupted
	}

	if (iNewNextOffset == aiSrcSize)
	{
		iNextOffset = 0; // at the edge
	}
	else
	{
		iNextOffset = iNewNextOffset;
	}		

	iType = iNewType;
	iLength = iNewLength;
	pcValPtr = (char*)pcPos;

	return true;
}




}

