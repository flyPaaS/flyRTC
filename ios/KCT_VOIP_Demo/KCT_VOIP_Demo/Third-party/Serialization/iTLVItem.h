#pragma once

namespace Comm
{

/// tlv项
class tTLVItem
{
public:
	int iType;			///< 类型
	int iLength;		///< 长度
	char *pcValPtr;		///< 值
	int iNextOffset;	///< 下个位置

	/// 构造函数
	tTLVItem();

	/// 析构函数
	virtual ~tTLVItem() {};

	/// 映射数据
	virtual bool MapTo(const char *apcSrc, const int aiSrcSize, const int aiOffset) = 0;
};

/// Util struct for traversing Fixed-Size TLV buffer
class tFixedSizeTLVItem : public tTLVItem
{
public:	
	/// Map this struct to the given buffer
	bool MapTo(const char *apcSrc, const int aiSrcSize, const int aiOffset);
};

/// Util struct for traversing Variable-Size TLV buffer
class tVariableSizeTLVItem : public tTLVItem
{
public:
	/// Map this struct to the given buffer
	bool MapTo(const char *apcSrc, const int aiSrcSize, const int aiOffset);
};

}

