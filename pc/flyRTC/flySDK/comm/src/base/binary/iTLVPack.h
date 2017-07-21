/** @file
	Encode and decode TLV format packages, cross platform supported.

@version 080804 Mal Initialized, suitable for not much fields, directly operate on buffer
**/

#ifndef __ITLVPACK_H__
#define __ITLVPACK_H__

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <map>

namespace Comm
{

/// Return code of all CTLVPack functions, >= 0 means succeeded, < 0 means error
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

/// TLV Package Encoder/Decoder
class CTLVPack
{
public:
	/// C-tor with default arg. specifying the initialize internal buffer size of tlv-pack, 0 for default: 1024
	CTLVPack(int aiInitBufSize = 0);
	
	/// destructor
	virtual ~CTLVPack();

public:
	enum eMode
	{
		TLV_FIXED_TL_SIZE = 0,
		TLV_VARIABLE_TL_SIZE = 1,
	};

public:
	/// Set the default mode
	/** @warning This method affects all object running in the process.	
	*/
	static void SetDefaultMode(eMode aeMode);

	/// Set the working mode
	/** There are currently 2 modes of tlvpack, the TLV_FIXED_TL_SIZE mode having a fixed size Type and Length
		field of 32bit integers. The TLV_VARIABLE_TL_SIZE mode using a variable byte algorithm to save some space
		of the final buffer by making the Type and Length fields variable length. If the data stored in the pack
		are always indexed between 1 to 127, and the data length is also under 127 bytes long, the Type field will
		take only 1 byte to representing, and the Length field will take 1 byte, too. Saving 6 bytes per data.

		@remark Setting a different mode when there is data within will DESTROY the data.
	*/
	void SetMode(eMode aeMode);

	/// Attach to a buffer
	/** @param apcBuf in, the buffer to attach to
		@param aiUsedSize in, USED size of the buffer
		@param aiAllocSize in, ALLOCATED size of the buffer

		@remark	The attaching buffer MUST be allocated using new.
				Caller MUST NOT DELETE the attached buffer.

		@note	If aiAllocSize == 0 (default), CTLVPack will treat the 
				attached buffer as if its AllocSize == UsedSize. 
				That is, if new data be added to the buffer, a new buffer 
				will be allocated to replace the old one, while the old 
				buffer will be DELETED by CTLVPack. 
	*/
	int Attach(const char *apcBuf, const int aiUsedSize, const int aiAllocSize = 0);


	/// Detach the TLV buffer
	/** @param appcBuf out, returning the detached buffer, can be NULL for detaching the previously attached buffer
		@param apiUsedSize out, returning the USED size of the detached buffer, can be NULL when appcBuf is NULL
		@param apiAllocSize out, returning the ALLOC size of the detached buffer,
				this param can be NULL(default) if you don't care about it.

		@remark Caller is responsible to DELETE the detached buffer.

		@attention	Detaching will leave the CTLVPack object in a state
					just like it's being created(nothing in it).					
	*/
	int Detach(char **appcBuf = NULL, int *apiUsedSize = NULL, int *apiAllocSize = NULL);


	/// Copying the TLV buffer to a caller-prepared buffer
	/** @param apcBuf out, caller-prepared buffer to hold the copy of TLV buffer
		@param apiSize in/out, input as the size of the caller-prepared buffer,
				output as the actual bytes written to it.

		@remark If *apiSize is less than the used size of the TLV buffer,
				return value will be TLVPRC_BUF_TO_SMALL, while *apiSize
				will contain the required size.	
	*/
	int CopyTo(char *apcBuf, int *apiSize);

	/// Copy from buffer 
	int CopyFrom(const char *apcBuf, int aiUsedSize, int aiAllocSize = 0);


	/// Get the used size of TLV buffer
	int GetUsedSize();

	/// Get the allocated size of TLV buffer
	int GetAllocSize();

	/// Erase all records
	void Clear();

	bool IsValidTLVPack(const char *apcBuf, const int aiUsedSize);
	

	//////////////////////////////////////////////////////////////////////////
	// Add Methods

	int AddChar(const int aiType, char acVal);
	int AddByte(const int aiType, unsigned char acVal);	
	int AddShort(const int aiType, short ahVal, bool abNetOrder = true);
	int AddWord(const int aiType, unsigned short awVal, bool abNetOrder = true);
	int AddInt(const int aiType, int aiVal, bool abNetOrder = true);
	int AddUInt(const int aiType, unsigned int auVal, bool abNetOrder = true);
	int AddLong(const int aiType, long alVal, bool abNetOrder = true);
	int AddDWord(const int aiType, unsigned long adwVal, bool abNetOrder = true);
	int AddLongLong(const int aiType, long long allVal, bool abNetOrder = true);
	int AddULongLong(const int aiType, unsigned long long aullVal, bool abNetOrder = true);
	int AddBool(const int aiType, bool abVal);
	int AddBuf(const int aiType, const char *apcVal, int aiLen);

	/// Add a TLVPack as a typed buffer
	/** @param aiType in, Type Id of the TLV
		@param apoVal in, TLVPack to add INT32o the TLV buffer
	
		@remark This method makes a copy of the given TLVPack's buffer.
	*/
	int AddNestedTLV(const int aiType, CTLVPack *apoVal);
	

	//////////////////////////////////////////////////////////////////////////
	// Get Methods

	int GetChar(const int aiType, char *apcVal);
	int GetByte(const int aiType, unsigned char *apcVal);	
	int GetShort(const int aiType, short *aphVal, bool abNetOrder = true);
	int GetWord(const int aiType, unsigned short *apwVal, bool abNetOrder = true);
	int GetInt(const int aiType, int *apiVal, bool abNetOrder = true);
	int GetUInt(const int aiType, unsigned int *apuVal, bool abNetOrder = true);
	int GetLong(const int aiType, long *aplVal, bool abNetOrder = true);
	int GetDWord(const int aiType, unsigned long *apdwVal, bool abNetOrder = true);
	int GetLongLong(const int aiType, long long *apllVal, bool abNetOrder = true);
	int GetULongLong(const int aiType, unsigned long long *apullVal, bool abNetOrder = true);
	int GetBool(const int aiType, bool *apbVal);

	/// Get a buffer from the TLV buffer
	/** @param aiType in, Type Id of the buffer
		@param apcVal in out, Buffer to copy to, size of *apiLen
		@param apiLen in out, input as the size of apcVal, output as the 
				actual bytes written to apcVal

		@remark If function failed as the RetCode is TLVPRC_BUF_TOO_SMALL,
				*apiLen will be the required size of the buffer.
				Actually, you can alway specifying apcVal = NULL and *apiLen
				= 0 to trigger a GetBufSize behavior.

				If somehow, the buffer is really of ZERO size while 
				apcVal = NULL and *apiLen = 0, it still return 
				TLVPRC_BUF_TOO_SMALL, AND, the *apiLen will be 0. You may
				then takes cares of it, or hand it to new/malloc as usual,
				it should does no harm.

		@warning GetBufSize behavior only activating when apcVal is NULL and
					*apiLen is 0. 
	*/
	int GetBuf(const int aiType, char *apcVal, int *apiLen);


	/// Get a buffer from the TLV buffer, and treat it like another TLV buffer 
	/** @param aiType in, Type id of the buffer
		@param appoVal out, returning a CTLVPack object to manage the buffer

		@remark Caller is responsible to DELETE the returned CTLVPack object.	
	*/
	int GetNestedTLVBuf(const int aiType, CTLVPack **appoVal);
	

	//////////////////////////////////////////////////////////////////////////
	// Size Methods 返回如果添加了这些东西所需的字节数(sizeT + sizeL + sizeV)

	static int SizeChar(const int aiType, eMode eTLVMode = TLV_FIXED_TL_SIZE);
	static int SizeByte(const int aiType, eMode eTLVMode = TLV_FIXED_TL_SIZE);	
	static int SizeShort(const int aiType, eMode eTLVMode = TLV_FIXED_TL_SIZE);
	static int SizeWord(const int aiType, eMode eTLVMode = TLV_FIXED_TL_SIZE);
	static int SizeInt(const int aiType, eMode eTLVMode = TLV_FIXED_TL_SIZE);
	static int SizeUInt(const int aiType, eMode eTLVMode = TLV_FIXED_TL_SIZE);
	static int SizeLong(const int aiType, eMode eTLVMode = TLV_FIXED_TL_SIZE);
	static int SizeDWord(const int aiType, eMode eTLVMode = TLV_FIXED_TL_SIZE);
	static int SizeLongLong(const int aiType, eMode eTLVMode = TLV_FIXED_TL_SIZE);
	static int SizeULongLong(const int aiType, eMode eTLVMode = TLV_FIXED_TL_SIZE);
	static int SizeBool(const int aiType, eMode eTLVMode = TLV_FIXED_TL_SIZE);
	static int SizeBuf(const int aiType, int aiLen, eMode eTLVMode = TLV_FIXED_TL_SIZE);
	static int SizeTLV(const int aiType, CTLVPack *apoVal, eMode eTLVMode = TLV_FIXED_TL_SIZE);
	static int SizeType(const int aiType = 0, eMode eTLVMode = TLV_FIXED_TL_SIZE);
	static int SizeLength(const int aiLen = -1, eMode eTLVMode = TLV_FIXED_TL_SIZE);
	static int SizeHeader();


public:
	class TLVBuffer
	{
	public:
		TLVBuffer()	{ m_pcBuf = NULL; }
		~TLVBuffer() { Reset(); }

		void SetBuffer(char *apcBuf, int aiUsedSize, int aiAllocSize, tagTLVPackHeader *aptHeader)
		{
			m_pcBuf = apcBuf;
			m_iUsedSize = aiUsedSize;
			m_iAllocSize = aiAllocSize;
			m_ptHeader = aptHeader;
		}

		void Reset()
		{
			m_pcBuf = NULL;
			m_iUsedSize = 0;
			m_iAllocSize = 0;
			m_ptHeader = NULL;
		}

		const char * GetBuffer() { return m_pcBuf; }
		int GetAllocSize() { return m_iAllocSize; }
		int GetUsedSize() { return m_iUsedSize; }
		const tagTLVPackHeader * GetHeader() { return m_ptHeader; }

	protected:
		char *m_pcBuf;
		int m_iUsedSize;
		int m_iAllocSize;
		tagTLVPackHeader *m_ptHeader;
	};

	/// 获取一个指向内部Buffer的对象，该对象内容在下一次写操作后失效
	TLVBuffer * GetInternalBuffer();	

	int GetIDMap( std::map< int, int > * apIDMap )const;

protected:
	void *m_pvInternal; //< implementation hidden mechanism
	char *m_pcBuf;
	int m_iAllocSize;
	int m_iUsedSize;
	tagTLVPackHeader *m_ptHeader;
	static eMode s_eDefaultMode;	

protected:
	int EnsureSpace(int aiType, int aiReqSize);

	unsigned short GetCheckSum(const char *apcBuf, const int aiSize);

	void DumpHeader(const tagTLVPackHeader *aptHeader);

	template <typename T> int AddNumber(const int aiType, T aoVal);
	template <typename T> int GetNumber(const int aiType, T *apoVal);
	template <typename T> static int SizeNumber(const int aiType, T aoVal, eMode eTLVMode);
};

}
#endif //__ITLVPACK_H__

