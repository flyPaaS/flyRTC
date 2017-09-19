
#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>

namespace Comm
{

/// TLV Package Encoder/Decoder
class SKTLVBuffer ;
class SKTLVPack
{
public:
	SKTLVPack(SKTLVBuffer * pTlvBuffer,const int &iIsToBuffer = 1);
	
	virtual ~SKTLVPack();
	
	int Init();
public:

	void SetHeaderPos(const int &aiHeaderPos);
	int GetHeaderPos();
	void SetLength(const int &aiLength);
	int GetLength();

	
	///////////////////////////////////////////////////////////////////
	// Get Methods
	
	int GetByte(const int &aiType, unsigned char *apcVal);	
	int GetWord(const int &aiType, unsigned short *apwVal);
	int GetUInt(const int &aiType, unsigned int *apuVal);
	int GetULongLong(const int &aiType, unsigned long long *apullVal);

	int GetBuf(const int &aiType, char *apcVal, int *apiLen);
	int GetNestedTLVBuf(const int &aiType, SKTLVPack **appoVal);

	SKTLVBuffer * GetTLVBuf();

	//////////////////////////////////////////////////////////////////////////
	// Add Methods
	int AddTL(const int &aiTpye, const int &aiLen);
	int AddByte(const int &aiType, unsigned char acVal);
	int AddWord(const int &aiType, unsigned short awVal);
	int AddUInt(const int &aiType, unsigned int auVal);
	int AddULongLong(const int &aiType, unsigned long long aullVal);

	int AddBuf(const int &aiType, const char *apcVal, int aiLen);
	int AddNestedTLV(SKTLVPack *aptTlvPack);

private:
	int SetHash();

protected:
	void *m_pvInternal;
};

}

