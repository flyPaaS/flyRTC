#include <stdlib.h>

#include "iSKTLVPack.h"
#include "iSKTLVBuffer.h"
#include "iTLVItem.h"
#include <string.h>
#include "iVByte.h"
#include "iHash.h"
#include "sktlvhelper.h"

#include "tlvport.h"



namespace Comm
{


const int c_iSizeofType = (int)sizeof(int);		///< Type is a Integer32
const int c_iSizeofLength = (int)sizeof(int);	///< Length is a Integer32
const int c_iInitializeSize = 1024;	///< Initialize buffer size
const int c_iBufGrowUnit = 1024;		///< Buffer grow unit, in bytes

struct tagTLVPackInternalData
{
	unsigned short *psCheckSum;

	char *pcBuf;
	int iAllocSize;
	int iUsedSize;

	int iIsCheckHeader;

	SKTLVBuffer::eMode eMode_t;

	tagTLVPackInternalData():psCheckSum(NULL),pcBuf(NULL),iIsCheckHeader(1){};
};

#define TLVMAGIC 0x81

#define HFunc_t TLVItemInfo_t::HFunc

#define CheckSum (((tagTLVPackInternalData*)m_pvInternal)->psCheckSum)
#define pcBuf_t (((tagTLVPackInternalData*)m_pvInternal)->pcBuf)
#define iAllocSize_t (((tagTLVPackInternalData*)m_pvInternal)->iAllocSize)
#define iUsedSize_t (((tagTLVPackInternalData*)m_pvInternal)->iUsedSize)
#define eMode_t (((tagTLVPackInternalData*)m_pvInternal)->eMode_t)
#define iIsCheckHeader_t (((tagTLVPackInternalData*)m_pvInternal)->iIsCheckHeader)

static char *func = "SKTLVBuffer";

typedef tagTLVPackHeader TLVHeader_t;

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

void SKTLVBuffer::SetMode(SKTLVBuffer::eMode eTLVMode)
{
	if (eTLVMode != TLV_FIXED_TL_SIZE &&
		eTLVMode != TLV_VARIABLE_TL_SIZE)
	{
		return;
	}

	eMode_t = eTLVMode;
}

SKTLVBuffer::eMode SKTLVBuffer :: GetMode()
{
	return eMode_t;
}


SKTLVBuffer::SKTLVBuffer(const int &aiInitSize,const int &aiIsCheckHeader) 
{
	
	m_pvInternal = new tagTLVPackInternalData;

	CheckSum = NULL;

	Clear();

	if( aiInitSize )
	{
		pcBuf_t = new char[aiInitSize];
		memset(pcBuf_t,0,aiInitSize*sizeof(char));

		iAllocSize_t = aiInitSize;
	}

	iIsCheckHeader_t = aiIsCheckHeader;

	eMode_t = TLV_VARIABLE_TL_SIZE;
}

SKTLVBuffer::~SKTLVBuffer(void)
{
	if (pcBuf_t != NULL)
	{
		delete[]pcBuf_t;
		pcBuf_t = NULL;
	}

	if( m_pvInternal != NULL )
	{
		if( CheckSum != NULL )
		{
			delete[] CheckSum;
			CheckSum = NULL;
		}

		delete (tagTLVPackInternalData*)m_pvInternal;
		m_pvInternal=NULL;
	}
}

void SKTLVBuffer::Clear()
{
	iUsedSize_t = 0;
	iAllocSize_t = 0; 
}


int SKTLVBuffer::Attach(
	const char *apcBuf, 
	const int &aiUsedSize
	)
{
	if (apcBuf == NULL || aiUsedSize == 0 || aiUsedSize < (int)sizeof(TLVHeader_t))
	{
		if( !SKTLVHelper :: CheckIsMM() )
		TLV_LOG_ERR ( "%s: attach error apcBuf(%p) usedsize(%d) ",func, apcBuf,aiUsedSize );
		return TLVPRC_ERR_INVALID_ARG;
	}
	
	if( iIsCheckHeader_t )
	{
		int ret = SetCheckSum(apcBuf,aiUsedSize);
		if( ret )
		{
		if( !SKTLVHelper :: CheckIsMM() )
			TLV_LOG_ERR ( "%s: check header error apcBuf(%p) usedsize(%d) ",func, apcBuf,aiUsedSize );
			return ret;
		}
	}

	int iHeaderPos = 0;

	pcBuf_t = (char *)apcBuf;

	iHeaderPos = iUsedSize_t;

	iAllocSize_t = aiUsedSize;
	iUsedSize_t = aiUsedSize;

	TLVHeader_t * ptHeader = (TLVHeader_t *)(pcBuf_t);
	SetMode((Comm::SKTLVBuffer::eMode)ptHeader->cMode);

	return iHeaderPos;
}

int SKTLVBuffer::Detach(char **appcBuf, int *apiUsedSize, int *apiAllocSize)
{
	if (NULL == appcBuf && apiUsedSize == NULL && apiAllocSize == NULL )
	{
		pcBuf_t = NULL;
		Clear();
		return TLVPRC_OK;
	}

	SetHeader(0,iUsedSize_t);

	*appcBuf = pcBuf_t;

	memcpy((char*)apiUsedSize, (char*)&iUsedSize_t, sizeof(int));
	memcpy((char*)apiAllocSize, (char*)&iAllocSize_t, sizeof(int));

	pcBuf_t = NULL;

	Clear();

	return TLVPRC_OK;
}

int SKTLVBuffer::EnsureSpace(int aiType, int aiReqSize, int IsHeader /* = 0 */ )
{
	int iActualReqSize = 0;
	
	if( IsHeader )
	{
		iActualReqSize = (int)sizeof(TLVHeader_t);
	}
	else
	{
		if (eMode_t == TLV_FIXED_TL_SIZE)
		{
			iActualReqSize = aiReqSize + c_iSizeofType + c_iSizeofLength;
		}
		else if (eMode_t == TLV_VARIABLE_TL_SIZE)
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
	}

	if (iActualReqSize < 0 ||
		iActualReqSize + iUsedSize_t < 0)
	{
		if( !SKTLVHelper :: CheckIsMM() )
		TLV_LOG_ERR ( "%s: ensure error ActualReqSize(%d) UsedSize(%d) ",func, iActualReqSize,iUsedSize_t);
		return TLVPRC_ERR_SIZE_TOO_LARGE;
	}

	if (iActualReqSize > iAllocSize_t - iUsedSize_t)
	{	
		int iNewSize = iAllocSize_t;

		iNewSize += ( iActualReqSize + iUsedSize_t + c_iBufGrowUnit -1 ) / c_iBufGrowUnit * c_iBufGrowUnit;
		
		if (iNewSize < 0)
		{
			iNewSize = iActualReqSize + iUsedSize_t;
		}

		char *pcNew = new char[iNewSize];

		if (NULL == pcNew)
		{
		if( !SKTLVHelper :: CheckIsMM() )
			TLV_LOG_ERR ( "%s: ensure error memory error ",func );
			return TLVPRC_ERR_NO_MEMORY;
		}

		memcpy((char*)pcNew, (char*)pcBuf_t, iUsedSize_t);
		memset((char*)pcNew+iUsedSize_t, 0, iNewSize-iUsedSize_t);

		iAllocSize_t = iNewSize;

		delete[]pcBuf_t;
		pcBuf_t = pcNew;
	}
	
	return TLVPRC_OK;
}

int SKTLVBuffer :: SetHash(const int &aiHeaderPos,const int &aiUsedSize,CHashTable<int,TLVItemInfo_t> *apHash)
{
	
	if( iIsCheckHeader_t )
	{
		if (false == IsValidTLVPack(pcBuf_t+aiHeaderPos, aiUsedSize,apHash))
		{
		if( !SKTLVHelper :: CheckIsMM() )
			TLV_LOG_ERR ( "%s: set hash error headerpos(%d) usedsize(%d)",func, aiHeaderPos, aiUsedSize );
			return TLVPRC_ERR_PACK_CORRUPTED;
		}
	}

	if( aiUsedSize == sizeof(TLVHeader_t) )
	{
		apHash->Init(0);
		//apHash->Clear();
		//empty
		return 0;
	}
		
	TLVHeader_t *pHeader = (tagTLVPackHeader*)(pcBuf_t+aiHeaderPos);

	tFixedSizeTLVItem tFixed;
	tVariableSizeTLVItem tVar;
	tTLVItem *pIter = NULL;
	
	if (pHeader->cMode == TLV_FIXED_TL_SIZE)
		pIter = &tFixed;
	else if (pHeader->cMode == TLV_VARIABLE_TL_SIZE)
		pIter = &tVar;
	else
	{
		if( !SKTLVHelper :: CheckIsMM() )
		TLV_LOG_ERR ( "%s: set hash error invalid headerpos(%d) usedsize(%d) mode(%d)",
			func, aiHeaderPos,aiUsedSize, pHeader->cMode );
		return -1;
	}

	if (false == pIter->MapTo(pcBuf_t+aiHeaderPos, aiUsedSize, sizeof(tagTLVPackHeader)))
	{
		if( !SKTLVHelper :: CheckIsMM() )
		TLV_LOG_ERR ( "%s: set hash error invalid tlv headerpos(%d) usedsize(%d) offset(%d) ",
			func, aiHeaderPos,aiUsedSize,pIter->iNextOffset );
		return -1;
	}

	int iTLVNum = 1;

	while (pIter->iNextOffset != 0)
	{
		++iTLVNum;
		if (false == pIter->MapTo(pcBuf_t+aiHeaderPos, aiUsedSize, pIter->iNextOffset))
		{
		if( !SKTLVHelper :: CheckIsMM() )
			TLV_LOG_ERR ( "%s: set hash error invalid tlv headerpos(%d) usedsize(%d) offset(%d) ",
				func, aiHeaderPos,aiUsedSize,pIter->iNextOffset );
			return -1;
		}
	}

	apHash->Init(iTLVNum);
	//apHash->Clear();

	pIter->MapTo(pcBuf_t+aiHeaderPos, aiUsedSize, sizeof(tagTLVPackHeader));
	apHash->Insert( pIter->iType, TLVItemInfo_t(pIter->iType,pIter->pcValPtr, pIter->iLength) );

	while (pIter->iNextOffset != 0)
	{
		pIter->MapTo(pcBuf_t+aiHeaderPos, aiUsedSize, pIter->iNextOffset);
		apHash->Insert( pIter->iType, TLVItemInfo_t(pIter->iType,pIter->pcValPtr, pIter->iLength) );
	}


	return 0;
}
bool SKTLVBuffer::IsValidTLVPack(const char *apcBuf, const int aiUsedSize,CHashTable<int,TLVItemInfo_t> *apHash)
{
//return 1;
	if (NULL == apcBuf)
	{
		if( !SKTLVHelper :: CheckIsMM() )
		TLV_LOG_ERR ( "%s: IsValidTLVPack error ", func );
		return false;
	}
	
	// header verify
	if (aiUsedSize < (int)sizeof(TLVHeader_t))
	{
		if( !SKTLVHelper :: CheckIsMM() )
		TLV_LOG_ERR ( "%s: IsValidTLVPack error header(%d) sizeof(%d) ", func, apcBuf-pcBuf_t, aiUsedSize );
		return false;
	}

	TLVHeader_t *pHeader = (tagTLVPackHeader*)apcBuf;

	int iBodySize = aiUsedSize - sizeof(TLVHeader_t);

	if (pHeader->cMagic != TLVMAGIC)
	{
		if( !SKTLVHelper :: CheckIsMM() )
		TLV_LOG_ERR ( "%s: IsValidTLVPack error header magic error header(%d) magic(%d) ",
				func, apcBuf-pcBuf_t, pHeader->cMagic);
		return false;
	}

	ToLittleEndian(&iBodySize, sizeof(int));
	if (0 != memcmp(&(pHeader->uSize), &iBodySize, sizeof(unsigned int)))
	{
		if( !SKTLVHelper :: CheckIsMM() )
		TLV_LOG_ERR ( "%s: IsValidTLVPack error size error header(%d) size(%d) Actually(%d)",
				func, apcBuf-pcBuf_t, pHeader->uSize,iBodySize );
		return false;
	}

	
	if (iBodySize == 0)
	{
		return true; // empty tlv pack
	}
	unsigned short wCheckSum = GetCheckSum(apcBuf + sizeof(TLVHeader_t), iBodySize);
	if (0 != memcmp((char*)&(pHeader->wCheckSum), (char*)&wCheckSum, sizeof(unsigned short)))
	{
		if( !SKTLVHelper :: CheckIsMM() )
		TLV_LOG_ERR ( "%s: IsValidTLVPack error checksum error header(%d) sum(%d) Actually(%d)",
				func, apcBuf-pcBuf_t, pHeader->wCheckSum,wCheckSum );
		return false;
	}

	return true;
}
int SKTLVBuffer::SetCheckSum(const char *apcBuf,const int &aiUsedSize)
{
	if( CheckSum != NULL )
	{
		// has got
		return TLVPRC_OK;
	}
	
	CheckSum = new unsigned short[aiUsedSize];
	

	if( CheckSum == NULL )
	{
		if( !SKTLVHelper :: CheckIsMM() )
		TLV_LOG_ERR ( "%s: SetCheckSum error memory error ", func );
		return TLVPRC_ERR_NO_MEMORY;
	}

	for(int i=0;i<aiUsedSize;++i)
	{
		if( i )
			CheckSum[i] = CheckSum[i-1]+(unsigned short)(signed char)apcBuf[i];
		else
			CheckSum[i] = (unsigned short)(signed char)apcBuf[i];
	}

	return TLVPRC_OK;
}
unsigned short SKTLVBuffer::GetCheckSum(const char *apcBuf, const int aiSize)
{
	if (NULL == apcBuf || 0 == aiSize)
	{
		if( !SKTLVHelper :: CheckIsMM() )
		TLV_LOG_ERR ( "%s: GetCheckSum error apcBuf(%p) size(%d)", func, apcBuf, aiSize );
		return 0xFFFF;
	}
	
	int iStartPos = apcBuf - pcBuf_t;
	int iEndPos = aiSize+iStartPos-1;

	unsigned short wCheckSum;
	if( iStartPos > 0 )
		wCheckSum = (CheckSum[iEndPos]-CheckSum[iStartPos-1]);
	else
		wCheckSum = CheckSum[iEndPos];

	wCheckSum = ~wCheckSum;

	ToLittleEndian(&wCheckSum, sizeof(unsigned short));

	return wCheckSum;
}

int SKTLVBuffer::GetUsedSize()
{
	return iUsedSize_t;
}

int SKTLVBuffer::GetAllocSize()
{
	return iAllocSize_t;
}




//////////////////////////////////////////////////////////////////////////
// Add Methods
int SKTLVBuffer :: AddHeader(int &aiLength)
{
	if (0 > EnsureSpace(0, sizeof(tagTLVPackHeader),1))
	{
		return TLVPRC_ERR_INIT;
	}
	
	TLVHeader_t * ptHeader = (TLVHeader_t *)(pcBuf_t + iUsedSize_t);
	ptHeader->cMagic = TLVMAGIC;
	ptHeader->cMode = eMode_t;

	int iHeaderPos = iUsedSize_t;

	iUsedSize_t += sizeof(TLVHeader_t);

	aiLength = sizeof(TLVHeader_t);

	return iHeaderPos;
}


void SKTLVBuffer :: SetHeader( const int &aiHeaderPos, const int &aiUsedSize )
{
	TLVHeader_t * ptHeader = (TLVHeader_t *)(pcBuf_t + aiHeaderPos);
	
	ptHeader->wCheckSum = ~ptHeader->wCheckSum;

	ToLittleEndian(&ptHeader->wCheckSum, sizeof(unsigned short));
	
	ptHeader->uSize = aiUsedSize-sizeof(TLVHeader_t);

	ToLittleEndian(&ptHeader->uSize, sizeof(unsigned int));
}

unsigned short SKTLVBuffer :: GetBodySum(const int &aiHeaderPos)
{
	TLVHeader_t * ptHeader = (TLVHeader_t *)(pcBuf_t + aiHeaderPos);

	return ptHeader->wCheckSum; 
}

int SKTLVBuffer :: AddBodySum(const unsigned short &aiVal, const int &aiHeaderPos )
{
	if( aiHeaderPos < 0 || aiHeaderPos > iUsedSize_t )
		return TLVPRC_ERR_INVALID_ARG;

	TLVHeader_t * ptHeader = (TLVHeader_t *)(pcBuf_t + aiHeaderPos);
	ptHeader->wCheckSum += (unsigned short)aiVal;	

	return TLVPRC_OK;
}

unsigned short SKTLVBuffer :: CountSum(const int &aiOldPos,const int &aiNewPos)
{
	unsigned short sum = 0;

	for(int i=aiOldPos;i<aiNewPos;++i)
		sum += ( unsigned short )( signed char )pcBuf_t[i];

	return sum;
}

int SKTLVBuffer :: AddTL(const int &aiType, const int &aiLen, const int &aiHeaderPos)
{
	int iOldUsedSize = iUsedSize_t;

	if (0 > EnsureSpace(aiType, aiLen))
	{
		return TLVPRC_ERR_INIT;
	}
	
	int iTmp = 0;
	if (eMode_t == TLV_FIXED_TL_SIZE)
	{
		iTmp = htonl(aiType);
		memcpy((char*)pcBuf_t + iUsedSize_t, (char*)&iTmp, c_iSizeofType);
		iUsedSize_t += sizeof(int);
		
		iTmp = htonl(aiLen);
		memcpy((char*)pcBuf_t + iUsedSize_t, (char*)&iTmp, c_iSizeofLength);
		iUsedSize_t += sizeof(int);
	}
	else if (eMode_t == TLV_VARIABLE_TL_SIZE)
	{
		int iTypeSize = EncodeVByte32((unsigned int)aiType, (unsigned char *)(pcBuf_t + iUsedSize_t));
		iUsedSize_t += iTypeSize;

		int iLengthSize = EncodeVByte32((unsigned int)aiLen, (unsigned char*)(pcBuf_t + iUsedSize_t));
		iUsedSize_t += iLengthSize;
	}
	else
	{
		return TLVPRC_ERR_INVALID_MODE;
	}
	
	int ret = AddBodySum(CountSum(iOldUsedSize,iUsedSize_t),aiHeaderPos);	

	if( ret >= 0 )
		return iUsedSize_t-iOldUsedSize;
	else
		return ret;

}

template<typename T>
int SKTLVBuffer::AddNumber(const int &aiType, T aoVal, const int &aiHeaderPos)
{
	if (0 > EnsureSpace(aiType, sizeof(T)))
	{
		return TLVPRC_ERR_INIT;
	}

	int iTmp = 0;
	
	int iOldUsedSize = iUsedSize_t;


	if (eMode_t == TLV_FIXED_TL_SIZE)
	{
		iTmp = htonl(aiType);
		memcpy((char*)pcBuf_t + iUsedSize_t, (char*)&iTmp, c_iSizeofType);
		iUsedSize_t += sizeof(int);
		
		iTmp = htonl(sizeof(T));
		memcpy((char*)pcBuf_t + iUsedSize_t, (char*)&iTmp, c_iSizeofLength);
		iUsedSize_t += sizeof(int);
	}
	else if (eMode_t == TLV_VARIABLE_TL_SIZE)
	{
		int iTypeSize = EncodeVByte32((unsigned int)aiType, (unsigned char *)(pcBuf_t + iUsedSize_t));
		iUsedSize_t += iTypeSize;

		int iLengthSize = EncodeVByte32((unsigned int)sizeof(T), (unsigned char*)(pcBuf_t + iUsedSize_t));
		iUsedSize_t += iLengthSize;
	}
	else
	{
		return TLVPRC_ERR_INVALID_MODE;
	}

	memcpy((char*)pcBuf_t + iUsedSize_t, (char*)&aoVal, sizeof(T));
	iUsedSize_t += sizeof(T);

	int ret = AddBodySum(CountSum(iOldUsedSize,iUsedSize_t),aiHeaderPos);	
	if( ret >= 0 )
		return iUsedSize_t-iOldUsedSize;
	else
		return ret;

}


int SKTLVBuffer::AddByte(const int &aiType, unsigned char acVal,const int &aiHeaderPos )
{
	return AddNumber(aiType, acVal,aiHeaderPos);
}

int SKTLVBuffer::AddWord(const int &aiType, unsigned short awVal,const int &aiHeaderPos)
{
	if (eMode_t == TLV_FIXED_TL_SIZE)
	{
		return AddNumber(aiType, (unsigned short)htons(awVal),aiHeaderPos);
	}
	else
	{
		return AddNumber(aiType, awVal,aiHeaderPos);
	}
}

int SKTLVBuffer::AddUInt( const int &aiType, unsigned int auVal, const int &aiHeaderPos )
{
	if (eMode_t == TLV_FIXED_TL_SIZE)
	{
		return AddNumber(aiType, (unsigned int)htonl(auVal),aiHeaderPos);
	}
	else
	{
		return AddNumber(aiType, auVal,aiHeaderPos);
	}	
}

int SKTLVBuffer::AddULongLong(const int &aiType, unsigned long long aullVal, const int &aiHeaderPos )
{
	if (eMode_t == TLV_FIXED_TL_SIZE)
	{
		return AddNumber(
			aiType, 
			((unsigned long long)(htonl((unsigned long)aullVal)) << 32) | (unsigned long long)htonl((unsigned long)(aullVal >> 32)),
			aiHeaderPos);
	}
	else
	{
		return AddNumber(aiType, aullVal,aiHeaderPos);
	}
}

int SKTLVBuffer::AddBuf(const int &aiType, const char *apcVal, int aiLen, const int &aiHeaderPos )
{
	if (aiLen < 0 || (NULL == apcVal && aiLen != 0))
	{
		return TLVPRC_ERR_INVALID_ARG;
	}

	if (0 > EnsureSpace(aiType, aiLen))
	{
		return TLVPRC_ERR_INIT;
	}
	
	int iOldUsedSize = iUsedSize_t;

	if (eMode_t == TLV_FIXED_TL_SIZE)
	{
		int iTmp = htonl(aiType);
		memcpy((char*)pcBuf_t + iUsedSize_t, (char*)&iTmp, c_iSizeofType);
		iUsedSize_t += c_iSizeofType;

		iTmp = htonl(aiLen);
		memcpy((char*)pcBuf_t + iUsedSize_t, (char*)&iTmp, c_iSizeofLength);
		iUsedSize_t += c_iSizeofLength;
	}
	else if (eMode_t == TLV_VARIABLE_TL_SIZE)
	{
		int iTypeSize = EncodeVByte32((unsigned int)aiType, (unsigned char *)(pcBuf_t + iUsedSize_t));
		iUsedSize_t += iTypeSize;

		int iLengthSize = EncodeVByte32((unsigned int)aiLen, (unsigned char *)(pcBuf_t + iUsedSize_t));
		iUsedSize_t += iLengthSize;
	}
	else
	{
		return TLVPRC_ERR_INVALID_MODE;
	}
	if (aiLen != 0)
	{
		memcpy((char*)&pcBuf_t[iUsedSize_t], (char*)apcVal, aiLen);
		iUsedSize_t += aiLen;
	}
	int ret = AddBodySum(CountSum(iOldUsedSize,iUsedSize_t),aiHeaderPos);	
	if( ret >= 0 )
		return iUsedSize_t-iOldUsedSize;
	else
		return ret;
}

int SKTLVBuffer :: AddNestedTLV(const int &aiHeaderPos,const int &aiSrcHeaderPos,const int &aiUsedSize)
{
	unsigned short iBodySum = GetBodySum(aiSrcHeaderPos);

	SetHeader(aiSrcHeaderPos,aiUsedSize);

	unsigned short iHeaderSum = GetHeaderSum(aiSrcHeaderPos);

	int ret = AddBodySum(iBodySum+iHeaderSum,aiHeaderPos);

	if( ret >= 0 )
		return aiUsedSize;

	return ret;
}

//////////////////////////////////////////////////////////////////////////
// Get Methods

template <typename T>
int SKTLVBuffer::GetNumber(const int &aiType, T *apoVal,const int &aiHeaderPos,const int &aiLength,CHashTable<int,TLVItemInfo_t>  *apHash)
{
	if (NULL == apoVal)
	{
		return TLVPRC_ERR_INVALID_ARG;
	}

	if (iUsedSize_t == sizeof(TLVHeader_t))
	{
		return TLVPRC_ERR_NOT_FOUND;
	}

	memset((char*)apoVal, 0, sizeof(T));

	TLVItemInfo_t ItemInfo;

	int ret = apHash->Find(aiType,ItemInfo);
	if( ret != 0 )
	{
		return TLVPRC_ERR_NOT_FOUND;
	}

	memcpy((char*)apoVal, (char*)ItemInfo.pcBuf, sizeof(T));

	return TLVPRC_OK;
}

int SKTLVBuffer::GetByte(const int &aiType, unsigned char *apcVal,const int &aiHeaderPos,const int &aiLength,
		CHashTable<int,TLVItemInfo_t> *apHash)
{
	return GetNumber(aiType, apcVal,aiHeaderPos,aiLength,apHash);
}

int SKTLVBuffer::GetWord(const int &aiType, unsigned short *apwVal,const int &aiHeaderPos,const int &aiLength,
		CHashTable<int,TLVItemInfo_t> *apHash)
{
	memset((char*)apwVal, 0, sizeof(unsigned short));

	unsigned short valTmp = 0;	

	int iRet = GetNumber(aiType, &valTmp,aiHeaderPos,aiLength,apHash);
	if (iRet < 0)
	{
		return iRet;
	}

	if (eMode_t == TLV_FIXED_TL_SIZE)
	{
		valTmp = (unsigned short)ntohs(valTmp);
	}

	memcpy((char*)apwVal, (char*)&valTmp, sizeof(unsigned short));

	return TLVPRC_OK;
}

int SKTLVBuffer::GetUInt(const int &aiType, unsigned int *apuVal,const int &aiHeaderPos,const int &aiLength,
		CHashTable<int,TLVItemInfo_t> *apHash)
{
	memset((char*)apuVal, 0, sizeof(unsigned int));

	unsigned int valTmp = 0;	

	int iRet = GetNumber(aiType, &valTmp,aiHeaderPos,aiLength,apHash);
	if (iRet < 0)
	{
		return iRet;
	}

	if (eMode_t == TLV_FIXED_TL_SIZE)
	{
		valTmp = (unsigned int)ntohl(valTmp);
	}

	memcpy((char*)apuVal, (char*)&valTmp, sizeof(unsigned int));

	return TLVPRC_OK;
}

int SKTLVBuffer::GetULongLong(const int &aiType, unsigned long long *apullVal,const int &aiHeaderPos,const int &aiLength,
		CHashTable<int,TLVItemInfo_t> *apHash)
{
	memset((char*)apullVal, 0, sizeof(long long));

	unsigned long long valTmp = 0;

	int iRet = GetNumber(aiType, &valTmp,aiHeaderPos,aiLength,apHash);
	if (iRet < 0)
	{
		return iRet;
	}

	if (eMode_t == TLV_FIXED_TL_SIZE)
	{
		valTmp = 
			((unsigned long long)(ntohl((unsigned long)valTmp)) << 32) | (unsigned long long)htonl((unsigned long)(valTmp >> 32));
	}

	memcpy((char*)apullVal, (char*)&valTmp, sizeof(unsigned long long));

	return TLVPRC_OK;
}

int SKTLVBuffer::GetBuf(const int &aiType, char *apcVal, int *apiLen,const int &aiHeaderPos,const int &aiLength,
		CHashTable<int,TLVItemInfo_t> *apHash)
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

	if (aiLength == sizeof(TLVHeader_t))
	{
		return TLVPRC_ERR_NOT_FOUND;
	}

	if (0 != iLen)
	{
		memset((char*)apcVal, 0, iLen);
	}

	TLVItemInfo_t ItemInfo;

	int ret = apHash->Find(aiType,ItemInfo);
	if( ret != 0 )
	{
		return TLVPRC_ERR_NOT_FOUND;
	}

	if (ItemInfo.iLen > iLen || NULL == apcVal)
	{
		memcpy((char*)apiLen, (char*)&(ItemInfo.iLen), sizeof(int));
		return TLVPRC_ERR_BUF_TOO_SMALL;
	}

	if (0 != ItemInfo.iLen)
	{
		memcpy((char*)apcVal, (char*)ItemInfo.pcBuf, ItemInfo.iLen);
	}
	memcpy((char*)apiLen, (char*)&(ItemInfo.iLen), sizeof(int));
	return TLVPRC_OK;
}

int SKTLVBuffer::GetNestedTLVBuf(const int &aiType, SKTLVPack **appoVal,const int &aiHeaderPos,const int &aiUsedSize,
		CHashTable<int,TLVItemInfo_t> *apHash)
{
	if (NULL == appoVal)
	{
		return TLVPRC_ERR_INVALID_ARG;
	}
	
	if ( iUsedSize_t < aiHeaderPos+aiUsedSize )
	{
		return TLVPRC_ERR_NOT_FOUND;
	}

	*appoVal = NULL;

	TLVItemInfo_t ItemInfo;

	int ret = apHash->Find(aiType,ItemInfo);
	if( ret != 0 )
	{   
		return TLVPRC_ERR_NOT_FOUND;
	}

	*appoVal = new SKTLVPack(this,0);
	
	if (ItemInfo.iLen> 0)
	{
		(*appoVal)->SetHeaderPos(ItemInfo.pcBuf-pcBuf_t);
		(*appoVal)->SetLength(ItemInfo.iLen);

		ret = (*appoVal)->Init();
		if( ret != 0 )
			return ret;
	}

	return TLVPRC_OK;
}

unsigned short SKTLVBuffer :: GetHeaderSum(const int &aiHeaderPos)
{
	unsigned short val=0;
	for(int i = 0;i < (int)sizeof(tagTLVPackHeader);++i)
	{
		val += ( unsigned short )( signed char )pcBuf_t[i+aiHeaderPos];	
	}
	return val;
}
//////////////////////////////////////////////////////////////////////////

int SKTLVBuffer::SizeBuf(const int &aiType, const int &aiLen, const eMode &eTLVMode)
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

int SKTLVBuffer :: SizeTLV(const int &aiBufLen, const eMode &eTLVMode)
{
	if (eTLVMode == TLV_FIXED_TL_SIZE)
	{
		return aiBufLen + SizeHeader();
	}

	if (eTLVMode == TLV_VARIABLE_TL_SIZE)
	{
		int iTmp;
		return EncodeVByte32((unsigned int)(aiBufLen+SizeHeader()), (unsigned char*)&iTmp);
	}
	return -1;
}

int SKTLVBuffer :: SizeBufWithTL(const int &aiType, const int &aiBufLen, const eMode &eTLVMode)
{
	if (eTLVMode == TLV_FIXED_TL_SIZE)
	{
		return c_iSizeofType + c_iSizeofLength + aiBufLen + SizeHeader();
	}

	if (eTLVMode == TLV_VARIABLE_TL_SIZE)
	{
		int iTmp = 0;
		int iTypeSize = EncodeVByte32((unsigned int)aiType, (unsigned char*)&iTmp);
		int iLengthSize = EncodeVByte32((unsigned int)(aiBufLen+SizeHeader()), (unsigned char*)&iTmp);
		return iTypeSize + iLengthSize + aiBufLen + SizeHeader(); 
	}

	return 0x7FFFFFFF; // let this number be great to let the user found error
}

int SKTLVBuffer :: SizeBuf(const int &aiLen) //buflen+header
{
	return aiLen + SizeHeader();
}

int SKTLVBuffer::SizeHeader()
{
	return (int)sizeof(TLVHeader_t);
}



}

