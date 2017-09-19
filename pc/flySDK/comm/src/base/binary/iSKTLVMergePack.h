#pragma once

#include "iTLVPack.h"

namespace Comm
{

typedef struct tagSKTLVMergeInfo SKTLVMergeInfo_t;
class CTLVPack;
class SKBuffer;
class SKTLVMergePack
{
	public:
	SKTLVMergePack();
	~SKTLVMergePack();
	public:
		int Init( const SKBuffer * poBuff1, const SKBuffer * poBuff2 );
		int GetTLVPack( const int &iFieldId, CTLVPack ** poTlvPack );
		int GetTLVPack( CTLVPack * &poTlvPack1, CTLVPack * &poTlvPack2 );
		int GetBaseTLVPack( const int &iFieldID, CTLVPack ** poTlvPack );
		int SetTLVPack( CTLVPack * poTlvPack1, CTLVPack * poTlvPack2 );
		int GetBuff( const int &iFieldID, char * &pcValue, int &iSize );
	private:
		int BufferInit( CTLVPack * &poTlvPack, const SKBuffer * poBuff );
	private:
		SKTLVMergeInfo_t * m_ptImpl;
};


}
