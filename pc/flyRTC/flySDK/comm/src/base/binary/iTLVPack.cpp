#include "iTLVPack.h"
#include "iTLVItem.h"
#include <string.h>
#include "iVByte.h"

#include "tlvport.h"
#include "sktlvhelper.h"

#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>


namespace Comm
{


const int c_iSizeofType = sizeof(int);		///< Type is a Integer32
const int c_iSizeofLength = sizeof(int);	///< Length is a Integer32
const int c_iInitializeSize = 1024;	///< Initialize buffer size
const int c_iBufGrowUnit = 1024;		///< Buffer grow unit, in bytes

// 内部实现数据，免除变更头文件带来的某些问题
struct tagTLVPackInternalData
{
	int iInitializeSize; ///< 初始化TLVPack大小
	CTLVPack::TLVBuffer m_bufRef;
};

#define TLVMAGIC 0x81

CTLVPack::eMode CTLVPack::s_eDefaultMode = CTLVPack::TLV_FIXED_TL_SIZE;

/// 历史原因的补救措施：转换为反网络字节序
/** 由于初版的疏漏，Header中的数值项没有转换为网络字节序。为了兼容，只好规定Header中的数值始终
    为反网络字节序。
	sigh...
*/
inline void ToLittleEndian(void *x, int len)
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


void CTLVPack::SetDefaultMode(CTLVPack::eMode aeMode)
{
	if (aeMode != TLV_FIXED_TL_SIZE &&
		aeMode != TLV_VARIABLE_TL_SIZE)
	{
		return;
	}

	s_eDefaultMode = aeMode;
}

CTLVPack::CTLVPack(int aiInitBufSize /*= 0*/) :
	m_pcBuf(NULL)
{
	m_pvInternal = (void *)new tagTLVPackInternalData;

	if (aiInitBufSize == 0)
	{
		((tagTLVPackInternalData*)m_pvInternal)->iInitializeSize = c_iInitializeSize;
	}
	else
	{
		((tagTLVPackInternalData*)m_pvInternal)->iInitializeSize = aiInitBufSize;
	}

	((tagTLVPackInternalData*)m_pvInternal)->m_bufRef.Reset();

	Clear();
	m_ptHeader->cMode = s_eDefaultMode;
}

CTLVPack::~CTLVPack(void)
{
	if (m_pcBuf != NULL)
	{
		((tagTLVPackInternalData*)m_pvInternal)->m_bufRef.Reset();
		delete[]m_pcBuf;
	}

	delete ((tagTLVPackInternalData*)m_pvInternal);	
}

void CTLVPack::Clear()
{
	tagTLVPackInternalData *pIntData = (tagTLVPackInternalData *)m_pvInternal;

	if (NULL != m_pcBuf)
	{
		pIntData->m_bufRef.Reset();
		delete[]m_pcBuf;
		m_pcBuf = NULL;
	}
	
	m_pcBuf = new char[pIntData->iInitializeSize];
	memset((char*)m_pcBuf, 0, pIntData->iInitializeSize);
	m_iAllocSize = pIntData->iInitializeSize;
	m_iUsedSize = sizeof(tagTLVPackHeader);

	m_ptHeader = (tagTLVPackHeader*)m_pcBuf;
	m_ptHeader->cMagic = TLVMAGIC;
	// clear doesn't changes the mode
	memset((char*)&(m_ptHeader->wCheckSum), 0, sizeof(m_ptHeader->wCheckSum));
	memset((char*)&(m_ptHeader->uSize), 0, sizeof(m_ptHeader->uSize));
}

void CTLVPack::SetMode(eMode aeMode)
{
	if (m_ptHeader->cMode == (unsigned char)aeMode)
	{
		return;
	}

	if (m_pcBuf != NULL)
	{
		((tagTLVPackInternalData*)m_pvInternal)->m_bufRef.Reset();
		delete[]m_pcBuf;
		m_pcBuf = NULL;
	}

	Clear();
	m_ptHeader->cMode = (unsigned char)aeMode;
}

int CTLVPack::EnsureSpace(int aiType, int aiReqSize)
{
	int iActualReqSize = 0;

	if (m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		iActualReqSize = aiReqSize + c_iSizeofType + c_iSizeofLength;
	}
	else if (m_ptHeader->cMode == TLV_VARIABLE_TL_SIZE)
	{
		int iTmp = 0;
		int iSizeofType = EncodeVByte32((unsigned int)aiType, (unsigned char*)&iTmp);
		int iSizeofLength = EncodeVByte32((unsigned int)aiReqSize, (unsigned char*)&iTmp);

		iActualReqSize = iSizeofType + iSizeofLength + aiReqSize;
	}
	else
	{
		return TLVPRC_ERR_INVALID_MODE;
	}

	if (iActualReqSize < 0 ||
		iActualReqSize + m_iUsedSize < 0)
	{
		return TLVPRC_ERR_SIZE_TOO_LARGE;
	}

	if (iActualReqSize > m_iAllocSize - m_iUsedSize)
	{	
		int iNewSize = m_iAllocSize;
		while (iNewSize < iActualReqSize + m_iUsedSize)
		{
			iNewSize += c_iBufGrowUnit;
		}

		if (iNewSize < 0)
		{
			iNewSize = iActualReqSize + m_iUsedSize;
		}

		char *pcNew = new char[iNewSize];
		if (NULL == pcNew)
		{
			return TLVPRC_ERR_NO_MEMORY;
		}

		memset((char*)pcNew, 0, iNewSize);
		memcpy((char*)pcNew, (char*)m_pcBuf, m_iUsedSize);
		m_iAllocSize = iNewSize;
		((tagTLVPackInternalData*)m_pvInternal)->m_bufRef.Reset();
		delete[]m_pcBuf;
		m_pcBuf = pcNew;
		m_ptHeader = (tagTLVPackHeader*)m_pcBuf;
	}
	
	return TLVPRC_OK;
}

bool CTLVPack::IsValidTLVPack(const char *apcBuf, const int aiUsedSize)
{
	if (NULL == apcBuf)
	{
		return false;
	}

	// header verify
	if ((int)aiUsedSize < (int)sizeof(tagTLVPackHeader))
	{
		return false;
	}

	tagTLVPackHeader *pHeader = (tagTLVPackHeader*)apcBuf;

	int iBodySize = aiUsedSize - sizeof(tagTLVPackHeader);

	if (pHeader->cMagic != TLVMAGIC)
	{
		DumpHeader(pHeader);
		return false;
	}

	int iBodySizeLE = iBodySize;
	ToLittleEndian(&iBodySizeLE, sizeof(int));
	if (0 != memcmp(&(pHeader->uSize), &iBodySizeLE, sizeof(unsigned int)))
	{
		TLV_LOG_ERR ( "TLVPACK: attach size failed %i", iBodySizeLE );
		DumpHeader(pHeader);
		return false;
	}

	unsigned short wCheckSum = GetCheckSum(apcBuf + sizeof(tagTLVPackHeader), iBodySize);
	if (0 != memcmp((char*)&(pHeader->wCheckSum), (char*)&wCheckSum, sizeof(unsigned short)))
	{
		TLV_LOG_ERR ( "TLVPACK: attach checksum failed %i size %i", wCheckSum, iBodySize );
		DumpHeader(pHeader);
		return false;
	}

	if (iBodySize == 0)
	{
		return true; // empty tlv pack
	}

	// body verify
	tFixedSizeTLVItem tFixed;
	tVariableSizeTLVItem tVar;
	tTLVItem *pIter = NULL;

	if (pHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		pIter = &tFixed;
	}
	else if (pHeader->cMode == TLV_VARIABLE_TL_SIZE)
	{
		pIter = &tVar;
	}
	else
	{
		DumpHeader(pHeader);
		return false;
	}	

	if (false == pIter->MapTo(apcBuf, aiUsedSize, sizeof(tagTLVPackHeader)))
	{
		return false;
	}

	while (pIter->iNextOffset != 0)
	{
		if (false == pIter->MapTo(apcBuf, aiUsedSize, pIter->iNextOffset))
		{
			return false;
		}
	}

	return true;
}

unsigned short CTLVPack::GetCheckSum(const char *apcBuf, const int aiSize)
{
	if (NULL == apcBuf || 0 == aiSize)
	{
		return 0xFFFF;
	}

	unsigned short wCheckSum = 0;
	for (int i = 0; i < aiSize; ++i)
	{
		wCheckSum += (unsigned short)(signed char)apcBuf[i];
	}

	wCheckSum = ~wCheckSum;

	ToLittleEndian(&wCheckSum, sizeof(unsigned short));

	return wCheckSum;
}

void CTLVPack::DumpHeader(const tagTLVPackHeader *aptHeader)
{
	if( !SKTLVHelper :: CheckIsMM() )
		TLV_LOG_ERR ( "TLVPACK: header magic %i mode %i checksum %i size %i reserved %i",
		aptHeader->cMagic, aptHeader->cMode, aptHeader->wCheckSum,
		aptHeader->uSize, aptHeader->uReserved );
}

int CTLVPack::Attach(
	const char *apcBuf, 
	const int aiUsedSize, 
	const int aiAllocSize /*= 0*/)
{
	if (apcBuf == NULL || aiUsedSize == 0)
	{
		return TLVPRC_ERR_INVALID_ARG;
	}

	if (aiAllocSize != 0 && aiAllocSize < aiUsedSize)
	{
		return TLVPRC_ERR_INVALID_ARG;
	}

	// verify buffer and calculate
	if (false == IsValidTLVPack(apcBuf, aiUsedSize))
	{
		return TLVPRC_ERR_PACK_CORRUPTED;
	}	
	
	if (m_pcBuf)
	{
		((tagTLVPackInternalData*)m_pvInternal)->m_bufRef.Reset();
		delete[] m_pcBuf;
	}
	
	m_pcBuf = (char *)apcBuf;
	m_ptHeader = (tagTLVPackHeader*)m_pcBuf;
	m_iAllocSize = (aiAllocSize == 0) ? aiUsedSize : aiAllocSize;
	m_iUsedSize = aiUsedSize;	

	return TLVPRC_OK;
}

int CTLVPack::Detach(char **appcBuf, int *apiUsedSize, int *apiAllocSize /*= NULL*/)
{
	if (NULL == appcBuf && NULL == apiUsedSize && NULL == apiAllocSize)
	{
		unsigned char cMode = m_ptHeader->cMode; // mode reserved
		m_pcBuf = NULL;
		Clear();
		m_ptHeader->cMode = cMode;
		return TLVPRC_OK;
	}

	if (NULL == appcBuf || NULL == apiUsedSize)
	{
		return TLVPRC_ERR_INVALID_ARG;
	}

	*appcBuf = NULL;
	memset((char*)apiUsedSize, 0, sizeof(int));
	if (NULL != apiAllocSize)
	{
		memset((char*)apiAllocSize, 0, sizeof(int));
	}

	*appcBuf = m_pcBuf;
	memcpy((char*)apiUsedSize, (char*)&m_iUsedSize, sizeof(int));
	if (NULL != apiAllocSize)
	{
		memcpy((char*)apiAllocSize, (char*)&m_iAllocSize, sizeof(int));
	}

	unsigned short wCheckSum = GetCheckSum(m_pcBuf+sizeof(tagTLVPackHeader), m_iUsedSize-sizeof(tagTLVPackHeader));
	memcpy((char*)&(m_ptHeader->wCheckSum), (char*)&wCheckSum, sizeof(unsigned short));

	unsigned int uSize = m_iUsedSize - sizeof(tagTLVPackHeader);
	ToLittleEndian(&uSize, sizeof(unsigned int));
	memcpy((char*)&(m_ptHeader->uSize), (char*)&uSize, sizeof(unsigned int));

	unsigned char cMode = m_ptHeader->cMode;

	((tagTLVPackInternalData*)m_pvInternal)->m_bufRef.Reset();
	m_pcBuf = NULL;
	Clear();
	m_ptHeader->cMode = cMode;
	
	return TLVPRC_OK;
}

int CTLVPack::CopyTo(char *apcBuf, int *apiSize)
{
	if (NULL == apcBuf || NULL == apiSize)
	{
		return TLVPRC_ERR_INVALID_ARG;
	}

	int iSize = 0;
	memcpy((char*)&iSize, (char*)apiSize, sizeof(int));

	if (iSize < m_iUsedSize)
	{
		memcpy((char*)apiSize, (char*)&m_iUsedSize, sizeof(int));
		return TLVPRC_ERR_BUF_TOO_SMALL;
	}

	unsigned short wCheckSum = GetCheckSum(m_pcBuf+sizeof(tagTLVPackHeader), m_iUsedSize-sizeof(tagTLVPackHeader));
	memcpy((char*)&(m_ptHeader->wCheckSum), (char*)&wCheckSum, sizeof(unsigned short));

	unsigned int uSize = m_iUsedSize - sizeof(tagTLVPackHeader);
	ToLittleEndian(&uSize, sizeof(unsigned int));
	memcpy((char*)&(m_ptHeader->uSize), (char*)&uSize, sizeof(unsigned int));

	if (m_iUsedSize > 0)
	{
		memcpy((char*)apcBuf, (char*)m_pcBuf, m_iUsedSize);
	}

	memcpy((char*)apiSize, (char*)&m_iUsedSize, sizeof(int));
	return TLVPRC_OK;
}

int CTLVPack::CopyFrom(const char *apcBuf, int aiUsedSize, int aiAllocSize /*= 0*/)
{
	if ((NULL == apcBuf  && aiUsedSize < (int)sizeof(tagTLVPackHeader)) ||
		(aiAllocSize != 0 && aiAllocSize < aiUsedSize) ||
		aiUsedSize < 0)
	{
		return TLVPRC_ERR_INVALID_ARG;
	}

	// verify buffer and calculate
	if (false == IsValidTLVPack(apcBuf, aiUsedSize))
	{
		return TLVPRC_ERR_PACK_CORRUPTED;
	}

	char *pcNewBuf = new char[aiUsedSize];
	if (NULL == pcNewBuf)
	{
		return TLVPRC_ERR_NO_MEMORY;
	}
	memcpy((char*)pcNewBuf, (char*)apcBuf, aiUsedSize);

	if (m_pcBuf)
	{
		((tagTLVPackInternalData*)m_pvInternal)->m_bufRef.Reset();
		delete[] m_pcBuf;
	}

	m_pcBuf = (char *)pcNewBuf;
	m_ptHeader = (tagTLVPackHeader*)m_pcBuf;
	m_iAllocSize = (aiAllocSize == 0) ? aiUsedSize : aiAllocSize;
	m_iUsedSize = aiUsedSize;

	return TLVPRC_OK;
}

int CTLVPack::GetUsedSize()
{
	return m_iUsedSize;
}

int CTLVPack::GetAllocSize()
{
	return m_iAllocSize;
}




//////////////////////////////////////////////////////////////////////////
// Add Methods

template <typename T>
int CTLVPack::AddNumber(const int aiType, T aoVal)
{
	if (0 > EnsureSpace(aiType, sizeof(T)))
	{
		return TLVPRC_ERR_INIT;
	}

	((tagTLVPackInternalData*)m_pvInternal)->m_bufRef.Reset();
	int iTmp = 0;
		
	if (m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		iTmp = htonl(aiType);
		memcpy((char*)m_pcBuf + m_iUsedSize, (char*)&iTmp, c_iSizeofType);
		m_iUsedSize += sizeof(int);
		
		iTmp = htonl(sizeof(T));
		memcpy((char*)m_pcBuf + m_iUsedSize, (char*)&iTmp, c_iSizeofLength);
		m_iUsedSize += sizeof(int);
	}
	else if (m_ptHeader->cMode == TLV_VARIABLE_TL_SIZE)
	{
		int iTypeSize = EncodeVByte32((unsigned int)aiType, (unsigned char *)(m_pcBuf + m_iUsedSize));
		m_iUsedSize += iTypeSize;

		int iLengthSize = EncodeVByte32((unsigned int)sizeof(T), (unsigned char*)(m_pcBuf + m_iUsedSize));
		m_iUsedSize += iLengthSize;
	}
	else
	{
		return TLVPRC_ERR_INVALID_MODE;
	}

	memcpy((char*)m_pcBuf + m_iUsedSize, (char*)&aoVal, sizeof(T));
	m_iUsedSize += sizeof(T);

	return TLVPRC_OK;
}


int CTLVPack::AddChar(const int aiType, char acVal)
{
	return AddNumber(aiType, acVal);
}

int CTLVPack::AddByte(const int aiType, unsigned char acVal)
{
	return AddNumber(aiType, acVal);
}

int CTLVPack::AddShort(const int aiType, short ahVal, bool abNetOrder /*= true*/)
{
	if (abNetOrder && m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		return AddNumber(aiType, (short)htons(ahVal));
	}
	else
	{
		return AddNumber(aiType, ahVal);
	}
}


int CTLVPack::AddWord(const int aiType, unsigned short awVal, bool abNetOrder /*= true*/)
{
	if (abNetOrder && m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		return AddNumber(aiType, (unsigned short)htons(awVal));
	}
	else
	{
		return AddNumber(aiType, awVal);
	}
}

int CTLVPack::AddInt(const int aiType, int aiVal, bool abNetOrder /*= true*/)
{
	if (abNetOrder && m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		return AddNumber(aiType, (int)htonl(aiVal));
	}
	else
	{
		return AddNumber(aiType, aiVal);
	}	
}

int CTLVPack::AddBool(const int aiType, bool abVal)
{
	if (abVal == false)
	{
		return AddNumber(aiType, (int)0);
	}
	else
	{
		return AddNumber(aiType, (int)1);
	}
}

int CTLVPack::AddUInt(const int aiType, unsigned int auVal, bool abNetOrder /*= true*/)
{
	if (abNetOrder && m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		return AddNumber(aiType, (unsigned int)htonl(auVal));
	}
	else
	{
		return AddNumber(aiType, auVal);
	}	
}


int CTLVPack::AddLong(const int aiType, long alVal, bool abNetOrder /*= true*/)
{
	if (abNetOrder && m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		return AddNumber(aiType, (long)htonl(alVal));
	}
	else
	{
		return AddNumber(aiType, alVal);
	}	
}


int CTLVPack::AddDWord(const int aiType, unsigned long adwVal, bool abNetOrder /*= true*/)
{
	if (abNetOrder && m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		return AddNumber(aiType, htonl(adwVal));
	}
	else
	{
		return AddNumber(aiType, adwVal);
	}	
}

int CTLVPack::AddLongLong(const int aiType, long long allVal, bool abNetOrder /*= true*/)
{
	if (abNetOrder && m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		return AddNumber(
			aiType, 
			((long long)(htonl((unsigned long)allVal)) << 32) | (long long)htonl((unsigned long)(allVal >> 32))
			);
	}
	else
	{
		return AddNumber(aiType, allVal);
	}
}

int CTLVPack::AddULongLong(const int aiType, unsigned long long aullVal, bool abNetOrder /*= true*/)
{
	if (abNetOrder && m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		return AddNumber(
			aiType, 
			((unsigned long long)(htonl((unsigned long)aullVal)) << 32) | (unsigned long long)htonl((unsigned long)(aullVal >> 32))
			);
	}
	else
	{
		return AddNumber(aiType, aullVal);
	}
}

int CTLVPack::AddBuf(const int aiType, const char *apcVal, int aiLen)
{
	if (aiLen < 0 || (NULL == apcVal && aiLen != 0))
	{
		return TLVPRC_ERR_INVALID_ARG;
	}

	((tagTLVPackInternalData*)m_pvInternal)->m_bufRef.Reset();
	if (0 > EnsureSpace(aiType, aiLen))
	{
		return TLVPRC_ERR_INIT;
	}

	if (m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		int iTmp = htonl(aiType);
		memcpy((char*)m_pcBuf + m_iUsedSize, (char*)&iTmp, c_iSizeofType);
		m_iUsedSize += c_iSizeofType;

		iTmp = htonl(aiLen);
		memcpy((char*)m_pcBuf + m_iUsedSize, (char*)&iTmp, c_iSizeofLength);
		m_iUsedSize += c_iSizeofLength;
	}
	else if (m_ptHeader->cMode == TLV_VARIABLE_TL_SIZE)
	{
		int iTypeSize = EncodeVByte32((unsigned int)aiType, (unsigned char *)(m_pcBuf + m_iUsedSize));
		m_iUsedSize += iTypeSize;

		int iLengthSize = EncodeVByte32((unsigned int)aiLen, (unsigned char *)(m_pcBuf + m_iUsedSize));
		m_iUsedSize += iLengthSize;
	}
	else
	{
		return TLVPRC_ERR_INVALID_MODE;
	}

	if (aiLen != 0)
	{
		memcpy((char*)&m_pcBuf[m_iUsedSize], (char*)apcVal, aiLen);
		m_iUsedSize += aiLen;
	}

	return TLVPRC_OK;
}

int CTLVPack::AddNestedTLV(const int aiType, CTLVPack *apoVal)
{
	if (NULL == apoVal)
	{
		return TLVPRC_ERR_INVALID_ARG;
	}

	((tagTLVPackInternalData*)m_pvInternal)->m_bufRef.Reset();
	if (0 > EnsureSpace(aiType, apoVal->m_iUsedSize))
	{
		return TLVPRC_ERR_INIT;
	}

	int iOldUsedSize = m_iUsedSize;

	if (m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		int iTmp = htonl(aiType);
		memcpy((char*)m_pcBuf + m_iUsedSize, (char*)&iTmp, c_iSizeofType);
		m_iUsedSize += c_iSizeofType;

		iTmp = htonl(apoVal->m_iUsedSize);
		memcpy((char*)m_pcBuf + m_iUsedSize, (char*)&iTmp, c_iSizeofLength);
		m_iUsedSize += c_iSizeofLength;
	}
	else if (m_ptHeader->cMode == TLV_VARIABLE_TL_SIZE)
	{
		int iTypeSize = EncodeVByte32((unsigned int)aiType, (unsigned char *)(m_pcBuf + m_iUsedSize));
		m_iUsedSize += iTypeSize;

		int iLengthSize = EncodeVByte32((unsigned int)apoVal->m_iUsedSize, (unsigned char *)(m_pcBuf + m_iUsedSize));
		m_iUsedSize += iLengthSize;
	}
	else
	{
		return TLVPRC_ERR_INVALID_MODE;
	}

	if (apoVal->m_iUsedSize > 0)
	{
		int iSize = m_iAllocSize - m_iUsedSize;
		int iRet = apoVal->CopyTo((char *)(m_pcBuf + m_iUsedSize), &iSize);
		if (TLVPRC_OK != iRet)
		{
			m_iUsedSize = iOldUsedSize;
			return iRet;
		}
	}	
	m_iUsedSize += apoVal->m_iUsedSize;
	return TLVPRC_OK;
}



//////////////////////////////////////////////////////////////////////////
// Get Methods

template <typename T>
int CTLVPack::GetNumber(const int aiType, T *apoVal)
{
	if (NULL == apoVal)
	{
		return TLVPRC_ERR_INVALID_ARG;
	}

	if (m_iUsedSize == sizeof(tagTLVPackHeader))
	{
		return TLVPRC_ERR_NOT_FOUND;
	}

	memset((char*)apoVal, 0, sizeof(T));

	tFixedSizeTLVItem tFixed;
	tVariableSizeTLVItem tVar;
	tTLVItem *pIter = NULL;

	if (m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		pIter = &tFixed;
	}
	else if (m_ptHeader->cMode == TLV_VARIABLE_TL_SIZE)
	{
		pIter = &tVar;
	}
	else
	{
		return TLVPRC_ERR_INVALID_MODE;
	}

	if (false == pIter->MapTo(m_pcBuf, m_iUsedSize, sizeof(tagTLVPackHeader)))
	{
		return TLVPRC_ERR_PACK_CORRUPTED;
	}

	while (pIter->iType != aiType && pIter->iNextOffset != 0)
	{
		if (false == pIter->MapTo(m_pcBuf, m_iUsedSize, pIter->iNextOffset))
		{
			return TLVPRC_ERR_PACK_CORRUPTED;
		}
	}

	if (pIter->iType != aiType)
	{
		return TLVPRC_ERR_NOT_FOUND;
	}

	memcpy((char*)apoVal, (char*)pIter->pcValPtr, sizeof(T));
	return TLVPRC_OK;
}

int CTLVPack::GetChar(const int aiType, char *apcVal)
{
	return GetNumber(aiType, apcVal);
}

int CTLVPack::GetByte(const int aiType, unsigned char *apcVal)
{
	return GetNumber(aiType, apcVal);
}

int CTLVPack::GetBool(const int aiType, bool *apbVal)
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

int CTLVPack::GetShort(const int aiType, short *aphVal, bool abNetOrder /*= true*/)
{
	memset((char*)aphVal, 0, sizeof(short));

	short valTmp = 0;
	int iRet = GetNumber(aiType, &valTmp);
	if (iRet < 0)
	{
		return iRet;
	}

	if (abNetOrder && m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		valTmp = (short)ntohs(valTmp);
	}

	memcpy((char*)aphVal, (char*)&valTmp, sizeof(short));

	return TLVPRC_OK;
}

int CTLVPack::GetWord(const int aiType, unsigned short *apwVal, bool abNetOrder /*= true*/)
{
	memset((char*)apwVal, 0, sizeof(unsigned short));

	unsigned short valTmp = 0;	
	int iRet = GetNumber(aiType, &valTmp);
	if (iRet < 0)
	{
		return iRet;
	}

	if (abNetOrder && m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		valTmp = (unsigned short)ntohs(valTmp);
	}

	memcpy((char*)apwVal, (char*)&valTmp, sizeof(unsigned short));

	return TLVPRC_OK;
}

int CTLVPack::GetInt(const int aiType, int *apiVal, bool abNetOrder /*= true*/)
{
	memset((char*)apiVal, 0, sizeof(int));

	int valTmp = 0;	
	int iRet = GetNumber(aiType, &valTmp);
	if (iRet < 0)
	{
		return iRet;
	}

	if (abNetOrder && m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		valTmp = (int)ntohl(valTmp);
	}

	memcpy((char*)apiVal, (char*)&valTmp, sizeof(int));

	return TLVPRC_OK;
}

int CTLVPack::GetUInt(const int aiType, unsigned int *apuVal, bool abNetOrder /*= true*/)
{
	memset((char*)apuVal, 0, sizeof(unsigned int));

	unsigned int valTmp = 0;	
	int iRet = GetNumber(aiType, &valTmp);
	if (iRet < 0)
	{
		return iRet;
	}

	if (abNetOrder && m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		valTmp = (unsigned int)ntohl(valTmp);
	}

	memcpy((char*)apuVal, (char*)&valTmp, sizeof(unsigned int));

	return TLVPRC_OK;
}

int CTLVPack::GetLong(const int aiType, long *aplVal, bool abNetOrder /*= true*/)
{
	memset((char*)aplVal, 0, sizeof(long));

	long valTmp = 0;	
	int iRet = GetNumber(aiType, &valTmp);
	if (iRet < 0)
	{
		return iRet;
	}

	if (abNetOrder && m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		valTmp = (long)ntohl(valTmp);
	}

	memcpy((char*)aplVal, (char*)&valTmp, sizeof(long));

	return TLVPRC_OK;
}

int CTLVPack::GetDWord(const int aiType, unsigned long *apdwVal, bool abNetOrder /*= true*/)
{
	memset((char*)apdwVal, 0, sizeof(unsigned long));

	unsigned long valTmp = 0;	
	int iRet = GetNumber(aiType, &valTmp);
	if (iRet < 0)
	{
		return iRet;
	}

	if (abNetOrder && m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		valTmp = (unsigned long)ntohl(valTmp);
	}

	memcpy((char*)apdwVal, (char*)&valTmp, sizeof(unsigned long));

	return TLVPRC_OK;
}

int CTLVPack::GetLongLong(const int aiType, long long *apllVal, bool abNetOrder /*= true*/)
{
	memset((char*)apllVal, 0, sizeof(long long));

	long long valTmp = 0;
	int iRet = GetNumber(aiType, &valTmp);
	if (iRet < 0)
	{
		return iRet;
	}

	if (abNetOrder && m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		valTmp = 
			((long long)(ntohl((unsigned long)valTmp)) << 32) | (long long)htonl((unsigned long)(valTmp >> 32));
	}

	memcpy((char*)apllVal, (char*)&valTmp, sizeof(long long));

	return TLVPRC_OK;
}

int CTLVPack::GetULongLong(const int aiType, unsigned long long *apullVal, bool abNetOrder /*= true*/)
{
	memset((char*)apullVal, 0, sizeof(long long));

	unsigned long long valTmp = 0;
	int iRet = GetNumber(aiType, &valTmp);
	if (iRet < 0)
	{
		return iRet;
	}

	if (abNetOrder && m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		valTmp = 
			((unsigned long long)(ntohl((unsigned long)valTmp)) << 32) | (unsigned long long)htonl((unsigned long)(valTmp >> 32));
	}

	memcpy((char*)apullVal, (char*)&valTmp, sizeof(unsigned long long));

	return TLVPRC_OK;
}

int CTLVPack::GetBuf(const int aiType, char *apcVal, int *apiLen)
{
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

	if (m_iUsedSize == sizeof(tagTLVPackHeader))
	{
		return TLVPRC_ERR_NOT_FOUND;
	}

	if (0 != iLen)
	{
		memset((char*)apcVal, 0, iLen);
	}

	tFixedSizeTLVItem tFixed;
	tVariableSizeTLVItem tVar;
	tTLVItem *pIter = NULL;

	if (m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		pIter = &tFixed;
	}
	else if (m_ptHeader->cMode == TLV_VARIABLE_TL_SIZE)
	{
		pIter = &tVar;
	}
	else
	{
		return TLVPRC_ERR_INVALID_MODE;
	}

	if (false == pIter->MapTo(m_pcBuf, m_iUsedSize, sizeof(tagTLVPackHeader)))
	{
		return TLVPRC_ERR_PACK_CORRUPTED;
	}

	while (pIter->iType != aiType && pIter->iNextOffset != 0)
	{
		if (false == pIter->MapTo(m_pcBuf, m_iUsedSize, pIter->iNextOffset))
		{
			return TLVPRC_ERR_PACK_CORRUPTED;
		}
	}

	if (pIter->iType != aiType)
	{
		return TLVPRC_ERR_NOT_FOUND;
	}

	if (pIter->iLength > iLen || NULL == apcVal)
	{
		memcpy((char*)apiLen, (char*)&(pIter->iLength), sizeof(int));
		return TLVPRC_ERR_BUF_TOO_SMALL;
	}

	if (0 != pIter->iLength)
	{
		memcpy((char*)apcVal, (char*)pIter->pcValPtr, pIter->iLength);
	}
	memcpy((char*)apiLen, (char*)&(pIter->iLength), sizeof(int));
	return TLVPRC_OK;
}

int CTLVPack::GetNestedTLVBuf(const int aiType, CTLVPack **appoVal)
{
	if (NULL == appoVal)
	{
		return TLVPRC_ERR_INVALID_ARG;
	}

	if (m_iUsedSize == sizeof(tagTLVPackHeader))
	{
		return TLVPRC_ERR_NOT_FOUND;
	}

	*appoVal = NULL;

	tFixedSizeTLVItem tFixed;
	tVariableSizeTLVItem tVar;
	tTLVItem *pIter = NULL;

	if (m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		pIter = &tFixed;
	}
	else if (m_ptHeader->cMode == TLV_VARIABLE_TL_SIZE)
	{
		pIter = &tVar;
	}
	else
	{
		return TLVPRC_ERR_INVALID_MODE;
	}

	if (false == pIter->MapTo(m_pcBuf, m_iUsedSize, sizeof(tagTLVPackHeader)))
	{
		return TLVPRC_ERR_PACK_CORRUPTED;
	}

	while (pIter->iType != aiType && pIter->iNextOffset != 0)
	{
		if (false == pIter->MapTo(m_pcBuf, m_iUsedSize, pIter->iNextOffset))
		{
			return TLVPRC_ERR_PACK_CORRUPTED;
		}
	}

	if (pIter->iType != aiType)
	{
		return TLVPRC_ERR_NOT_FOUND;
	}

	*appoVal = new CTLVPack;
	if (pIter->iLength > 0)
	{
		if (0 > (*appoVal)->CopyFrom(pIter->pcValPtr, pIter->iLength))
		{
			delete *appoVal;
			*appoVal = NULL;
			return TLVPRC_ERR_PACK_CORRUPTED;
		}
	}

	return TLVPRC_OK;
}

//////////////////////////////////////////////////////////////////////////

template <typename T> 
int CTLVPack::SizeNumber(const int aiType, T aoVal, eMode eTLVMode)
{
	if (eTLVMode == TLV_FIXED_TL_SIZE)
	{
		return c_iSizeofType + c_iSizeofLength + sizeof(T);
	}

	if (eTLVMode == TLV_VARIABLE_TL_SIZE)
	{
		int iTmp = 0;
		int iTypeSize = EncodeVByte32((unsigned int)aiType, (unsigned char*)&iTmp);
		int iLengthSize = EncodeVByte32((unsigned int)sizeof(T), (unsigned char*)&iTmp);
		return iTypeSize + iLengthSize + sizeof(T);
	}

	return 0x7FFFFFFF; // let this number be great to let the user found error
}

int CTLVPack::SizeChar(const int aiType, eMode eTLVMode/* = TLV_FIXED_TL_SIZE*/)
{
	return SizeNumber(aiType, (char)1, eTLVMode);
}

int CTLVPack::SizeByte(const int aiType, eMode eTLVMode/* = TLV_FIXED_TL_SIZE*/)
{
	return SizeNumber(aiType, (unsigned char)1, eTLVMode);
}

int CTLVPack::SizeShort(const int aiType, eMode eTLVMode/* = TLV_FIXED_TL_SIZE*/)
{
	return SizeNumber(aiType, (short)1, eTLVMode);
}

int CTLVPack::SizeWord(const int aiType, eMode eTLVMode/* = TLV_FIXED_TL_SIZE*/)
{
	return SizeNumber(aiType, (unsigned short)1, eTLVMode);
}

int CTLVPack::SizeInt(const int aiType, eMode eTLVMode/* = TLV_FIXED_TL_SIZE*/)
{
	return SizeNumber(aiType, (int)1, eTLVMode);
}

int CTLVPack::SizeUInt(const int aiType, eMode eTLVMode/* = TLV_FIXED_TL_SIZE*/)
{
	return SizeNumber(aiType, (unsigned int)1, eTLVMode);
}

int CTLVPack::SizeLong(const int aiType, eMode eTLVMode/* = TLV_FIXED_TL_SIZE*/)
{
	return SizeNumber(aiType, (long)1, eTLVMode);
}

int CTLVPack::SizeDWord(const int aiType, eMode eTLVMode/* = TLV_FIXED_TL_SIZE*/)
{
	return SizeNumber(aiType, (unsigned long)1, eTLVMode);
}

int CTLVPack::SizeLongLong(const int aiType, eMode eTLVMode/* = TLV_FIXED_TL_SIZE*/)
{
	return SizeNumber(aiType, (long long)1, eTLVMode);
}

int CTLVPack::SizeULongLong(const int aiType, eMode eTLVMode/* = TLV_FIXED_TL_SIZE*/)
{
	return SizeNumber(aiType, (unsigned long long)1, eTLVMode);
}

int CTLVPack::SizeBool(const int aiType, eMode eTLVMode/* = TLV_FIXED_TL_SIZE*/)
{
	return SizeNumber(aiType, (int)1, eTLVMode);
}

int CTLVPack::SizeBuf(const int aiType, int aiLen, eMode eTLVMode/* = TLV_FIXED_TL_SIZE*/)
{
	if (eTLVMode == TLV_FIXED_TL_SIZE)
	{
		return c_iSizeofType + c_iSizeofLength + aiLen;
	}

	if (eTLVMode == TLV_VARIABLE_TL_SIZE)
	{
		int iTmp = 0;
		int iTypeSize = EncodeVByte32((unsigned int)aiType, (unsigned char*)&iTmp);
		int iLengthSize = EncodeVByte32((unsigned int)aiLen, (unsigned char*)&iTmp);
		return iTypeSize + iLengthSize + aiLen;
	}

	return 0x7FFFFFFF; // let this number be great to let the user found error
}

int CTLVPack::SizeTLV(const int aiType, CTLVPack *apoVal, eMode eTLVMode/* = TLV_FIXED_TL_SIZE*/)
{
	if (eTLVMode == TLV_FIXED_TL_SIZE)
	{
		return c_iSizeofType + c_iSizeofLength + apoVal->m_iUsedSize;
	}

	if (eTLVMode == TLV_VARIABLE_TL_SIZE)
	{
		int iTmp = 0;
		int iTypeSize = EncodeVByte32((unsigned int)aiType, (unsigned char*)&iTmp);
		int iLengthSize = EncodeVByte32((unsigned int)apoVal->m_iUsedSize, (unsigned char*)&iTmp);
		return iTypeSize + iLengthSize + apoVal->m_iUsedSize;
	}

	return 0x7FFFFFFF; // let this number be great to let the user found error
}

int CTLVPack::SizeType(const int aiType /*= 0*/, eMode eTLVMode/* = TLV_FIXED_TL_SIZE*/)
{
	if (eTLVMode == TLV_FIXED_TL_SIZE)
	{
		return c_iSizeofType;
	}

	if (eTLVMode == TLV_VARIABLE_TL_SIZE)
	{
		if (aiType == 0)
		{
			return 0x7FFFFFFF; // let this number be great to let the user found error
		}

		int iTmp = 0;
		int iTypeSize = EncodeVByte32((unsigned int)aiType, (unsigned char*)&iTmp);

		return iTypeSize;
	}

	return 0x7FFFFFFF; // let this number be great to let the user found error
}

int CTLVPack::SizeLength(const int aiLen/* = -1*/, eMode eTLVMode/* = TLV_FIXED_TL_SIZE*/)
{
	if (eTLVMode == TLV_FIXED_TL_SIZE)
	{
		return c_iSizeofLength;
	}

	if (eTLVMode == TLV_VARIABLE_TL_SIZE)
	{
		if (aiLen == -1)
		{
			return 0x7FFFFFFF; // let this number be great to let the user found error
		}

		int iTmp = 0;
		int iLengthSize = EncodeVByte32((unsigned int)aiLen, (unsigned char*)&iTmp);

		return iLengthSize;
	}

	return 0x7FFFFFFF; // let this number be great to let the user found error
}

int CTLVPack::SizeHeader()
{
	return (int)sizeof(tagTLVPackHeader);
}

CTLVPack::TLVBuffer * CTLVPack::GetInternalBuffer()
{
	if (((tagTLVPackInternalData*)m_pvInternal)->m_bufRef.GetBuffer() == NULL)
	{
		((tagTLVPackInternalData*)m_pvInternal)->m_bufRef.SetBuffer(m_pcBuf, m_iUsedSize, m_iAllocSize, m_ptHeader);
	}

	return &(((tagTLVPackInternalData*)m_pvInternal)->m_bufRef);
};

int CTLVPack :: GetIDMap( std::map< int, int > * apIDMap )const
{
	tFixedSizeTLVItem tFixed;
	tVariableSizeTLVItem tVar;
	tTLVItem *pIter = NULL;

	if (m_ptHeader->cMode == TLV_FIXED_TL_SIZE)
	{
		pIter = &tFixed;
	}
	else if (m_ptHeader->cMode == TLV_VARIABLE_TL_SIZE)
	{
		pIter = &tVar;
	}
	else
	{
		return TLVPRC_ERR_INVALID_MODE;
	}

	if (false == pIter->MapTo(m_pcBuf, m_iUsedSize, sizeof(tagTLVPackHeader)))
	{
		return TLVPRC_ERR_PACK_CORRUPTED;
	}

	apIDMap->insert( std::make_pair( pIter->iType, pIter->iLength ) );

	while (pIter->iNextOffset != 0)
	{
		if (false == pIter->MapTo(m_pcBuf, m_iUsedSize, pIter->iNextOffset))
		{
			return TLVPRC_ERR_PACK_CORRUPTED;
		}
		apIDMap->insert( std::make_pair( pIter->iType, pIter->iLength ) );
	}

	return TLVPRC_OK;
}


}

