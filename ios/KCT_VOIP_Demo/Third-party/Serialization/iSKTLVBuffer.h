
/*
@note before getbodysum, mustn't to setheader and just setheader for each header only once
*/

#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>

namespace Comm
{

class SKTLVPack;
template<typename Ket_t,typename TLVItemInfo_t>
class CHashTable;

typedef struct tagTLVItemInfo
{
	int iType;
	const char * pcBuf;
	int iLen;

	tagTLVItemInfo(){}
	tagTLVItemInfo(const int &aiType,const char *apcBuf, const int &aiLen):iType(aiType),pcBuf(apcBuf),iLen(aiLen){};

	static unsigned int HFunc(const int &key)
	{
		return key;
	}

}TLVItemInfo_t;


/// Return code of all SKTLVBuffer functions, >= 0 means succeeded, < 0 means error
enum emTLVPackRetCode
{
	TLVPRC_OK = 0,						///< ok
	
	TLVPRC_ERR_NO_MEMORY = -1,			///< error:内存不够
	TLVPRC_ERR_INIT = -2,				///< error:初始化失败
	TLVPRC_ERR_SIZE_TOO_LARGE = -3,		///< error:长度太长
	TLVPRC_ERR_INVALID_ARG = -4,		///< error:参数非法
	TLVPRC_ERR_PACK_CORRUPTED = -5,		///< error:打包错误
	TLVPRC_ERR_NOT_FOUND = -6,			///< error:找不到
	TLVPRC_ERR_BUF_TOO_SMALL = -7,		///< error:buffer太小
	TLVPRC_ERR_INVALID_MODE = -8,		///< error:非法模式
};

#pragma pack(1)
/// tlv包头
struct tagTLVPackHeader
{
	unsigned char cMagic;			///< magic
	unsigned char cMode;			///< 模式
	unsigned short wCheckSum;		///< 校验
	unsigned int uSize;				///< 长度
	unsigned int uReserved;			///< 保留值
};
#pragma pack()

class SKTLVBuffer
{
public:
	SKTLVBuffer(const int &aiInitBufSize,const int &iIsCheckHeader = 1);
	
	virtual ~SKTLVBuffer();

public:
	enum eMode
	{
		TLV_FIXED_TL_SIZE = 0,
		TLV_VARIABLE_TL_SIZE = 1,
	};

public:
	void SetMode(eMode aeMode);

	eMode GetMode();

	int Attach(const char *apcBuf, const int &aiUsedSize);

	int Detach(char **appcBuf, int *apiUsedSize , int *apiAllocSize );

	int GetUsedSize();

	int GetAllocSize();
	
	int SetHash(const int &aiHeaderPos,const int &aiUsedSize,CHashTable<int,TLVItemInfo_t>  *hash);

	//////////////////////////////////////////////////////////////////////////
	// Add Methods
	int AddHeader(int &length);
	int AddTL(const int &aiTpye, const int &aiLen, const int &aiHeaderPos);

	int AddByte(const int &aiType, unsigned char acVal, const int &aiHeaderPos);	
	int AddWord(const int &aiType, unsigned short awVal, const int &aiHeaderPos);
	int AddUInt(const int &aiType, unsigned int auVal, const int &aiHeaderPos);
	int AddULongLong(const int &aiType, unsigned long long aullVal, const int &aiHeaderPos);

	int AddBuf(const int &aiType, const char *apcVal, int aiLen, const int &aiHeaderPos);
	int AddNestedTLV(const int &aiHeaderPos,const int &aiSrcHeaderPos,const int &aiSrcLength);
	//////////////////////////////////////////////////////////////////////////
	// Get Methods

	int GetByte(const int &aiType, unsigned char *apcVal,const int &aiHeaderPos,const int &aiLength,
			CHashTable<int,TLVItemInfo_t>  *hash);	
	int GetUInt(const int &aiType, unsigned int *apuVal,const int &aiHeaderPos,const int &aiLength,
			CHashTable<int,TLVItemInfo_t>  *hash);
	int GetWord(const int &aiType, unsigned short *apwVal,const int &aiHeaderPos,const int &aiLength,
			CHashTable<int,TLVItemInfo_t> *hash);
	int GetULongLong(const int &aiType, unsigned long long *apullVal,const int &aiHeaderPos,const int &aiLength,
			CHashTable<int,TLVItemInfo_t>  *hash);

	int GetBuf(const int &aiType, char *apcVal, int *apiLen,const int &aiHeaderPos,const int &aiLength,
			CHashTable<int,TLVItemInfo_t>  *hash);
	int GetNestedTLVBuf(const int &aiType, SKTLVPack **appoVal,const int &aiHeaderPos,const int &aiUsedSize,
			CHashTable<int,TLVItemInfo_t>  *hash);

	//type+len+buflen
	static int SizeBuf(const int &aiType, const int &aiLen, const eMode &eTLVMode = TLV_VARIABLE_TL_SIZE );
	//buflen+header
	static int SizeBuf(const int &aiLen); 
	//len+header_size
	static int SizeTLV(const int &aiBufLen, const eMode &eTLVMode = TLV_VARIABLE_TL_SIZE );
	//type+len+header
	static int SizeBufWithTL(const int &aiType, const int &aiBufLen, const eMode &eTLVMode = TLV_VARIABLE_TL_SIZE );
	//header
	static int SizeHeader();

protected:

	void *m_pvInternal; //< implementation hidden mechanism

protected:

	int EnsureSpace( int aiType, int aiReqSize, int IsHead=0 );

	void Clear();

	int SetCheckSum(const char *apcBuf,const int &aiUsedSize);
	unsigned short GetCheckSum(const char *apcBuf, const int aiSize);


	int AddBodySum(const unsigned short &aiVal, const int &aiHeaderPos );
	unsigned short GetBodySum(const int &aiHeaderPos);

	unsigned short GetHeaderSum(const int &aiHeaderPos);
	void SetHeader( const int &aiHeaderPos, const int &aiUsedSize );

	unsigned short CountSum(const int &aiOldPos,const int &aiNewPos);

	bool IsValidTLVPack(const char *apcBuf, const int aiUsedSize,CHashTable<int,TLVItemInfo_t>  *hash);

	template <typename T> int AddNumber(const int &aiType, T aoVal, const int &aiHeaderPos);
	template <typename T> int GetNumber(const int &aiType, T *apoVal,const int &aiHeaderPos,const int &aiLength,
			CHashTable<int,TLVItemInfo_t>  *hash);
};


}
