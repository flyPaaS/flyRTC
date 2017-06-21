#include "iSKTLVPack.h"
#include "iSKTLVBuffer.h"
#include "iTLVItem.h"
#include <string.h>
#include "iVByte.h"

#include "tlvport.h"
#include "skmetainfo.h"

#include "iHash.h"

namespace Comm
{

// 内部实现数据，免除变更头文件带来的某些问题
struct tagTLVPackInternalData
{
	CHashTable<int,TLVItemInfo_t>  *hash; 
	int m_iHeaderPos;
	int m_iLength;
	SKTLVBuffer *m_ptTlvBuffer;
};

#define hash_t (((tagTLVPackInternalData*)m_pvInternal)->hash)
#define iHeaderPos_t (((tagTLVPackInternalData*)m_pvInternal)->m_iHeaderPos)
#define iLength_t (((tagTLVPackInternalData*)m_pvInternal)->m_iLength)
#define ptTlvBuffer_t (((tagTLVPackInternalData*)m_pvInternal)->m_ptTlvBuffer)

#define HFunc_t TLVItemInfo_t::HFunc

SKTLVPack::SKTLVPack(SKTLVBuffer * pTlvBuffer,const int &iIsToBuffer/*=1*/) 
{
	m_pvInternal = new tagTLVPackInternalData;
	hash_t = NULL;
	ptTlvBuffer_t = pTlvBuffer;
	iHeaderPos_t = 0;
	iLength_t = 0;
	if( iIsToBuffer )
	{
		iHeaderPos_t = ptTlvBuffer_t->AddHeader(iLength_t);
	}
	else
	{
		hash_t = new CHashTable<int,TLVItemInfo_t>(HFunc_t,0);
		iLength_t = pTlvBuffer->GetUsedSize();
		//SetHash();
	}
}

int SKTLVPack :: Init()
{
	return SetHash();
}

SKTLVPack::~SKTLVPack(void)
{
	if( m_pvInternal != NULL )
	{
		if( hash_t != NULL )
			delete hash_t;
		delete (tagTLVPackInternalData*)m_pvInternal;
		m_pvInternal=NULL;
	}	
}

SKTLVBuffer *SKTLVPack :: GetTLVBuf()
{
	return ptTlvBuffer_t;
}

int SKTLVPack :: AddNestedTLV(SKTLVPack *aptTlvPack)
{
    int ret = ptTlvBuffer_t->AddNestedTLV(iHeaderPos_t,aptTlvPack->GetHeaderPos(),aptTlvPack->GetLength());
    if( ret >= 0 )
    {
        iLength_t += ret;
        return 0;
    }
    return ret;
}

void SKTLVPack :: SetHeaderPos(const int &aiHeaderPos)
{
	iHeaderPos_t = aiHeaderPos;
}

int SKTLVPack :: GetHeaderPos()
{
	return iHeaderPos_t ;
}
void SKTLVPack :: SetLength(const int &aiLength)
{
	iLength_t = aiLength;
}
int SKTLVPack :: GetLength()
{
	return iLength_t;
}

int SKTLVPack::SetHash(
	)
{
	// verify buffer and calculate
	
	int ret = ptTlvBuffer_t->SetHash(iHeaderPos_t,iLength_t,hash_t);

	if( ret < 0 )
	{
		return TLVPRC_ERR_PACK_CORRUPTED;
	}

	return TLVPRC_OK;
}


int SKTLVPack::GetByte(const int &aiType, unsigned char *apcVal)
{
	return ptTlvBuffer_t->GetByte(aiType,apcVal,iHeaderPos_t,iLength_t,hash_t);
}
int SKTLVPack::GetWord(const int &aiType, unsigned short *apwVal)
{
	return ptTlvBuffer_t->GetWord(aiType,apwVal,iHeaderPos_t,iLength_t,hash_t);
}
int SKTLVPack::GetUInt(const int &aiType, unsigned int *apuVal)
{
	return ptTlvBuffer_t->GetUInt(aiType,apuVal,iHeaderPos_t,iLength_t,hash_t);
}
int SKTLVPack::GetULongLong(const int &aiType, unsigned long long *apullVal)
{
	return ptTlvBuffer_t->GetULongLong(aiType,apullVal,iHeaderPos_t,iLength_t,hash_t);
}

int SKTLVPack::AddTL(const int &aiType, const int &aiLen)
{
	int ret = ptTlvBuffer_t->AddTL(aiType,aiLen,iHeaderPos_t);
	if( ret >= 0 )
	{
		iLength_t += ret;
		return 0;
	}
	return ret;
}


int SKTLVPack::AddByte(const int &aiType, unsigned char acVal)
{
	int ret = ptTlvBuffer_t->AddByte(aiType,acVal,iHeaderPos_t);
	if( ret >= 0 )
	{
		iLength_t += ret;
		return 0;
	}
	return ret;
}

int SKTLVPack::AddWord(const int &aiType, unsigned short awVal)
{
	int ret = ptTlvBuffer_t->AddWord(aiType,awVal,iHeaderPos_t);
	if( ret >= 0 )
	{
		iLength_t += ret;
		return 0;
	}
	return ret;
}

int SKTLVPack::AddUInt(const int &aiType, unsigned int auVal)
{
	int ret = ptTlvBuffer_t->AddUInt(aiType,auVal,iHeaderPos_t);
	if( ret >= 0 )
	{
		iLength_t += ret;
		return 0;
	}
	return ret;
}



int SKTLVPack::AddULongLong(const int &aiType, unsigned long long aullVal)
{
	int ret = ptTlvBuffer_t->AddULongLong(aiType,aullVal,iHeaderPos_t);
	if( ret >= 0 )
	{
		iLength_t += ret;
		return 0;
	}
	return ret;
}


int SKTLVPack::AddBuf(const int &aiType, const char *apcVal, int aiLen)
{
	int ret = ptTlvBuffer_t->AddBuf(aiType,apcVal,aiLen,iHeaderPos_t);
	if( ret >= 0 )
	{
		iLength_t += ret;
		return 0;
	}
	return ret;
}

int SKTLVPack::GetBuf(const int &aiType, char *apcVal, int *apiLen)
{
	return ptTlvBuffer_t->GetBuf(aiType,apcVal,apiLen,iHeaderPos_t,iLength_t,hash_t);
}

int SKTLVPack::GetNestedTLVBuf(const int &aiType, SKTLVPack **appoVal)
{
	return ptTlvBuffer_t->GetNestedTLVBuf(aiType,appoVal,iHeaderPos_t,iLength_t,hash_t);
}

}

