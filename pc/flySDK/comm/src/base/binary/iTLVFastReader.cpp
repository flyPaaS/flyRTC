#include "iTLVFastReader.h"
#include "iTLVItem.h"
#include <string.h>
#include <stdlib.h>
#include <map>

namespace Comm
{

struct tagTLVNode
{
	int iId;
	int iLen;
	const char *pcVal;
};

typedef std::map<int, tagTLVNode> Id2NodeMap;

struct tagTLVFastReaderInternal
{
	CTLVPack::TLVBuffer *pTLVBuf;
	Id2NodeMap mapId2BegPtr;
};

TLVFastReader::TLVFastReader() :
	m_pInternal(NULL)
{
	m_pInternal = new tagTLVFastReaderInternal;

	((tagTLVFastReaderInternal *)m_pInternal)->pTLVBuf = NULL;
}

TLVFastReader :: ~TLVFastReader()
{
	delete (tagTLVFastReaderInternal*)m_pInternal, m_pInternal = NULL;
}

int TLVFastReader::SetTLVPack(CTLVPack *apTLV)
{
	tagTLVFastReaderInternal *pData = (tagTLVFastReaderInternal *)m_pInternal;

	pData->pTLVBuf = apTLV->GetInternalBuffer();

	return CreateIndex();
}

int TLVFastReader::CreateIndex()
{
	tagTLVFastReaderInternal *pData = (tagTLVFastReaderInternal *)m_pInternal;

	if (pData->pTLVBuf == NULL) 
	{
		return __LINE__;
	}

	if (pData->pTLVBuf->GetUsedSize() == sizeof(tagTLVPackHeader))
	{
		return 0;
	}

	const char *pcTLVBuf = pData->pTLVBuf->GetBuffer();
	if (NULL == pcTLVBuf)
	{
		return __LINE__;
	}

	pData->mapId2BegPtr.clear();

	tFixedSizeTLVItem tFixed;
	tVariableSizeTLVItem tVar;
	tTLVItem *pIter = NULL;

	switch (pData->pTLVBuf->GetHeader()->cMode)
	{
	case CTLVPack::TLV_FIXED_TL_SIZE:
		{
			pIter = &tFixed;
		}
		break;
	case CTLVPack::TLV_VARIABLE_TL_SIZE:
		{
			pIter = &tVar;
		}
		break;
	default:
		return __LINE__;
	}	
	
	if (false == pIter->MapTo(pcTLVBuf, pData->pTLVBuf->GetUsedSize(), sizeof(tagTLVPackHeader)))
	{
		return __LINE__;
	}

	tagTLVNode &node = pData->mapId2BegPtr[pIter->iType];
	node.iId = pIter->iType;
	node.iLen = pIter->iLength;
	node.pcVal = pIter->pcValPtr;

	while (pIter->iNextOffset != 0) 
	{
		if (false == pIter->MapTo(pcTLVBuf, pData->pTLVBuf->GetUsedSize(), pIter->iNextOffset))
		{
			return __LINE__;
		}
		
		tagTLVNode &node = pData->mapId2BegPtr[pIter->iType];
		node.iId = pIter->iType;
		node.iLen = pIter->iLength;
		node.pcVal = pIter->pcValPtr;
	} 

	return 0;
}

template <typename T>
int TLVFastReader::GetNumber(const int aiType, T *apoVal)
{
	tagTLVFastReaderInternal *pData = (tagTLVFastReaderInternal *)m_pInternal;
	if (pData->pTLVBuf == NULL || pData->pTLVBuf->GetBuffer() == NULL)
	{
		abort();
	}

	if (NULL == apoVal)
	{
		return TLVPRC_ERR_INVALID_ARG;
	}

	memset((char*)apoVal, 0, sizeof(T));

	Id2NodeMap::iterator iterFound = pData->mapId2BegPtr.find(aiType);
	if (iterFound == pData->mapId2BegPtr.end())
	{
		return TLVPRC_ERR_NOT_FOUND;
	}
	
	memcpy((char*)apoVal, (char*)iterFound->second.pcVal, sizeof(T));

	return TLVPRC_OK;
}

int TLVFastReader::GetChar(const int aiType, char *apcVal)
{
	return GetNumber(aiType, apcVal);
}

int TLVFastReader::GetByte(const int aiType, unsigned char *apcVal)
{
	return GetNumber(aiType, apcVal);
}

int TLVFastReader::GetBool(const int aiType, bool *apbVal)
{
	int i = -1;
	int iRet = GetNumber(aiType, &i);
	if (iRet < 0)
	{
		return iRet;
	}

	if (i == 0)
	{
		*apbVal = false;
	}
	else
	{
		*apbVal = true;
	}

	return TLVPRC_OK;
}

int TLVFastReader::GetShort(const int aiType, short *aphVal, bool abNetOrder /*= true*/)
{
	tagTLVFastReaderInternal *pData = (tagTLVFastReaderInternal *)m_pInternal;
	memset((char*)aphVal, 0, sizeof(short));

	short valTmp = 0;
	int iRet = GetNumber(aiType, &valTmp);
	if (iRet < 0)
	{
		return iRet;
	}

	if (abNetOrder && pData->pTLVBuf->GetHeader()->cMode == CTLVPack::TLV_FIXED_TL_SIZE)
	{
		valTmp = (short)ntohs(valTmp);
	}

	memcpy((char*)aphVal, (char*)&valTmp, sizeof(short));

	return TLVPRC_OK;
}

int TLVFastReader::GetWord(const int aiType, unsigned short *apwVal, bool abNetOrder /*= true*/)
{
	tagTLVFastReaderInternal *pData = (tagTLVFastReaderInternal *)m_pInternal;
	memset((char*)apwVal, 0, sizeof(unsigned short));

	unsigned short valTmp = 0;	
	int iRet = GetNumber(aiType, &valTmp);
	if (iRet < 0)
	{
		return iRet;
	}

	if (abNetOrder && pData->pTLVBuf->GetHeader()->cMode == CTLVPack::TLV_FIXED_TL_SIZE)
	{
		valTmp = (unsigned short)ntohs(valTmp);
	}

	memcpy((char*)apwVal, (char*)&valTmp, sizeof(unsigned short));

	return TLVPRC_OK;
}

int TLVFastReader::GetInt(const int aiType, int *apiVal, bool abNetOrder /*= true*/)
{
	tagTLVFastReaderInternal *pData = (tagTLVFastReaderInternal *)m_pInternal;
	memset((char*)apiVal, 0, sizeof(int));

	int valTmp = 0;	
	int iRet = GetNumber(aiType, &valTmp);
	if (iRet < 0)
	{
		return iRet;
	}

	if (abNetOrder && pData->pTLVBuf->GetHeader()->cMode == CTLVPack::TLV_FIXED_TL_SIZE)
	{
		valTmp = (int)ntohl(valTmp);
	}

	memcpy((char*)apiVal, (char*)&valTmp, sizeof(int));

	return TLVPRC_OK;
}

int TLVFastReader::GetUInt(const int aiType, unsigned int *apuVal, bool abNetOrder /*= true*/)
{
	tagTLVFastReaderInternal *pData = (tagTLVFastReaderInternal *)m_pInternal;
	memset((char*)apuVal, 0, sizeof(unsigned int));

	unsigned int valTmp = 0;	
	int iRet = GetNumber(aiType, &valTmp);
	if (iRet < 0)
	{
		return iRet;
	}

	if (abNetOrder && pData->pTLVBuf->GetHeader()->cMode == CTLVPack::TLV_FIXED_TL_SIZE)
	{
		valTmp = (unsigned int)ntohl(valTmp);
	}

	memcpy((char*)apuVal, (char*)&valTmp, sizeof(unsigned int));

	return TLVPRC_OK;
}

int TLVFastReader::GetLong(const int aiType, long *aplVal, bool abNetOrder /*= true*/)
{
	tagTLVFastReaderInternal *pData = (tagTLVFastReaderInternal *)m_pInternal;
	memset((char*)aplVal, 0, sizeof(long));

	long valTmp = 0;	
	int iRet = GetNumber(aiType, &valTmp);
	if (iRet < 0)
	{
		return iRet;
	}

	if (abNetOrder && pData->pTLVBuf->GetHeader()->cMode == CTLVPack::TLV_FIXED_TL_SIZE)
	{
		valTmp = (long)ntohl(valTmp);
	}

	memcpy((char*)aplVal, (char*)&valTmp, sizeof(long));

	return TLVPRC_OK;
}

int TLVFastReader::GetDWord(const int aiType, unsigned long *apdwVal, bool abNetOrder /*= true*/)
{
	tagTLVFastReaderInternal *pData = (tagTLVFastReaderInternal *)m_pInternal;
	memset((char*)apdwVal, 0, sizeof(unsigned long));

	unsigned long valTmp = 0;	
	int iRet = GetNumber(aiType, &valTmp);
	if (iRet < 0)
	{
		return iRet;
	}

	if (abNetOrder && pData->pTLVBuf->GetHeader()->cMode == CTLVPack::TLV_FIXED_TL_SIZE)
	{
		valTmp = (unsigned long)ntohl(valTmp);
	}

	memcpy((char*)apdwVal, (char*)&valTmp, sizeof(unsigned long));

	return TLVPRC_OK;
}

int TLVFastReader::GetLongLong(const int aiType, long long *apllVal, bool abNetOrder /*= true*/)
{
	tagTLVFastReaderInternal *pData = (tagTLVFastReaderInternal *)m_pInternal;
	memset((char*)apllVal, 0, sizeof(long long));

	long long valTmp = 0;
	int iRet = GetNumber(aiType, &valTmp);
	if (iRet < 0)
	{
		return iRet;
	}

	if (abNetOrder && pData->pTLVBuf->GetHeader()->cMode == CTLVPack::TLV_FIXED_TL_SIZE)
	{
		valTmp = 
			((long long)(ntohl((unsigned long)valTmp)) << 32) | (long long)htonl((unsigned long)(valTmp >> 32));
	}

	memcpy((char*)apllVal, (char*)&valTmp, sizeof(long long));

	return TLVPRC_OK;
}

int TLVFastReader::GetULongLong(const int aiType, unsigned long long *apullVal, bool abNetOrder /*= true*/)
{
	tagTLVFastReaderInternal *pData = (tagTLVFastReaderInternal *)m_pInternal;
	memset((char*)apullVal, 0, sizeof(long long));

	unsigned long long valTmp = 0;
	int iRet = GetNumber(aiType, &valTmp);
	if (iRet < 0)
	{
		return iRet;
	}

	if (abNetOrder && pData->pTLVBuf->GetHeader()->cMode == CTLVPack::TLV_FIXED_TL_SIZE)
	{
		valTmp = 
			((unsigned long long)(ntohl((unsigned long)valTmp)) << 32) | (unsigned long long)htonl((unsigned long)(valTmp >> 32));
	}

	memcpy((char*)apullVal, (char*)&valTmp, sizeof(unsigned long long));

	return TLVPRC_OK;
}

int TLVFastReader::GetBuf(const int aiType, char *apcVal, int *apiLen)
{
	tagTLVFastReaderInternal *pData = (tagTLVFastReaderInternal *)m_pInternal;
	if (pData->pTLVBuf == NULL || pData->pTLVBuf->GetBuffer() == NULL)
	{
		abort();
	}

	if (NULL == apiLen)
	{
		return TLVPRC_ERR_INVALID_ARG;
	}

	int iLen = 0;
	memcpy((char*)&iLen, (char*)apiLen, sizeof(int));
	if (NULL == apcVal && 0 != iLen)
	{
		return TLVPRC_ERR_INVALID_ARG;
	}

	if (0 != iLen)
	{
		memset((char*)apcVal, 0, iLen);
	}

	Id2NodeMap::iterator iterFound = pData->mapId2BegPtr.find(aiType);
	if (iterFound == pData->mapId2BegPtr.end())
	{
		return TLVPRC_ERR_NOT_FOUND;
	}

	if (iterFound->second.iLen > iLen || NULL == apcVal)
	{
		memcpy((char*)apiLen, (char*)&(iterFound->second.iLen), sizeof(int));
		return TLVPRC_ERR_BUF_TOO_SMALL;
	}

	if (0 != iterFound->second.iLen)
	{
		memcpy((char*)apcVal, (char*)iterFound->second.pcVal, iterFound->second.iLen);
	}
	memcpy((char*)apiLen, (char*)&(iterFound->second.iLen), sizeof(int));
	return TLVPRC_OK;
}

int TLVFastReader::GetNestedTLVBuf(const int aiType, CTLVPack **appoVal)
{
	tagTLVFastReaderInternal *pData = (tagTLVFastReaderInternal *)m_pInternal;
	if (pData->pTLVBuf == NULL || pData->pTLVBuf->GetBuffer() == NULL)
	{
		abort();
	}

	if (NULL == appoVal)
	{
		return TLVPRC_ERR_INVALID_ARG;
	}

	*appoVal = NULL;

	Id2NodeMap::iterator iterFound = pData->mapId2BegPtr.find(aiType);
	if (iterFound == pData->mapId2BegPtr.end())
	{
		return TLVPRC_ERR_NOT_FOUND;
	}

	*appoVal = new CTLVPack;
	if (iterFound->second.iLen > 0)
	{
		if (0 > (*appoVal)->CopyFrom(iterFound->second.pcVal, iterFound->second.iLen))
		{
			delete *appoVal;
			*appoVal = NULL;
			return TLVPRC_ERR_PACK_CORRUPTED;
		}
	}

	return TLVPRC_OK;
}


}
