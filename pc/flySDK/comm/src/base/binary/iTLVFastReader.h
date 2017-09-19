#ifndef __ITLVFASTREADER_H__
#define __ITLVFASTREADER_H__

#include "iTLVPack.h"

namespace Comm
{

/// TLVPack高性能随机读取器
/** 适用于字段非常多的TLVPack随机读取，在初始化以及TLVPack每一次写操作后的首次读取会重新组织索引，
	会慢一点，之后如果一直都是读操作就会很快了。
*/
class TLVFastReader
{
public:
	/// 构造函数
	TLVFastReader();
	~TLVFastReader();

	/// 设置tlv pack
	int SetTLVPack(CTLVPack *apTLV);

	/// get char
	int GetChar(const int aiType, char *apcVal);

	/// get byte
	int GetByte(const int aiType, unsigned char *apcVal);	

	/// get short
	int GetShort(const int aiType, short *aphVal, bool abNetOrder = true);

	/// get word
	int GetWord(const int aiType, unsigned short *apwVal, bool abNetOrder = true);

	/// get int
	int GetInt(const int aiType, int *apiVal, bool abNetOrder = true);

	/// get unsigned int
	int GetUInt(const int aiType, unsigned int *apuVal, bool abNetOrder = true);

	/// get long
	int GetLong(const int aiType, long *aplVal, bool abNetOrder = true);

	/// get unsigned long
	int GetDWord(const int aiType, unsigned long *apdwVal, bool abNetOrder = true);

	/// get long long
	int GetLongLong(const int aiType, long long *apllVal, bool abNetOrder = true);

	/// get unsigned long long
	int GetULongLong(const int aiType, unsigned long long *apullVal, bool abNetOrder = true);

	/// get bool
	int GetBool(const int aiType, bool *apbVal);

	/// get buffer
	int GetBuf(const int aiType, char *apcVal, int *apiLen);

	/// get nested tlv buffer
	int GetNestedTLVBuf(const int aiType, CTLVPack **appoVal);

protected:
	void *m_pInternal;
	
protected:
	int CreateIndex();
	template <typename T> int GetNumber(const int aiType, T *apoVal);
};

}
#endif /* __ITLVFASTREADER_H__ */

