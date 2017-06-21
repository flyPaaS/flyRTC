
#include "iSKTLVMergePack.h"
#include <stdlib.h>
#include "skbuffer.h"
#include "tlvport.h"

namespace Comm
{
	typedef struct tagSKTLVMergeInfo 
	{
		CTLVPack * poTlvPack1;	
		CTLVPack * poTlvPack2;	
		bool bIsOwn;
	}SKTLVMergeInfo_t;

	SKTLVMergePack :: SKTLVMergePack()
	{
		m_ptImpl = new SKTLVMergeInfo_t;
		m_ptImpl->poTlvPack1 = NULL;
		m_ptImpl->poTlvPack2 = NULL;
		m_ptImpl->bIsOwn = true;
	}

	SKTLVMergePack :: ~SKTLVMergePack()
	{
		if( m_ptImpl )
		{
			if( m_ptImpl->bIsOwn )
			{
				if( m_ptImpl->poTlvPack1 )
				{
					m_ptImpl->poTlvPack1->Detach(NULL, NULL, NULL);
					delete m_ptImpl->poTlvPack1;
					m_ptImpl->poTlvPack1 = NULL;
				}

				if( m_ptImpl->poTlvPack2 )
				{
					m_ptImpl->poTlvPack2->Detach(NULL, NULL, NULL);
					delete m_ptImpl->poTlvPack2;
					m_ptImpl->poTlvPack2 = NULL;
				}
			}
			delete m_ptImpl;
			m_ptImpl = NULL;
		}
	}

	int SKTLVMergePack :: BufferInit( CTLVPack * &poTlvBuffer, const SKBuffer * poBuffer )
	{
		int iRet = poTlvBuffer->Attach((char *)poBuffer->GetBuffer(), poBuffer->GetLen());
		return iRet;
	}

	int SKTLVMergePack :: Init( const SKBuffer * poBuff1, const SKBuffer * poBuff2 )
	{
		m_ptImpl->poTlvPack1 = new CTLVPack;
		int ret = BufferInit( m_ptImpl->poTlvPack1, poBuff1 );
		if( ret )
		{
			delete m_ptImpl->poTlvPack1,m_ptImpl->poTlvPack1 = NULL;
			LogErr("%s buff1 init error ret %d",__func__, ret);
			return ret;
		}
		
		m_ptImpl->poTlvPack2 = new CTLVPack;
		ret = BufferInit( m_ptImpl->poTlvPack2, poBuff2 );
		if( ret )
		{
			delete m_ptImpl->poTlvPack2, m_ptImpl->poTlvPack2 = NULL;
			LogErr("%s buff2 init error ret %d",__func__, ret);
			return ret;
		}

		return 0;
	}

	int SKTLVMergePack :: SetTLVPack( CTLVPack * poTlvPack1, CTLVPack * poTlvPack2 )
	{
		m_ptImpl->bIsOwn = false;
		m_ptImpl->poTlvPack1 = poTlvPack1;
		m_ptImpl->poTlvPack2 = poTlvPack2;
		return 0;
	}

	int SKTLVMergePack :: GetTLVPack( CTLVPack * &poTlvPack1, CTLVPack * &poTlvPack2 )
	{
		poTlvPack1 = m_ptImpl->poTlvPack1;
		poTlvPack2 = m_ptImpl->poTlvPack2;
		return 0;
	}

	int SKTLVMergePack :: GetTLVPack( const int &iFieldId, CTLVPack ** poTlvPack )
	{
		int iRet = 0;
		iRet = m_ptImpl->poTlvPack2->GetNestedTLVBuf ( iFieldId, poTlvPack );
		if( iRet != 0 )
		{
			LogDebug("%s buff1 get nested error ret %d",__func__, iRet);
			iRet = m_ptImpl->poTlvPack1->GetNestedTLVBuf ( iFieldId, poTlvPack );
			if( iRet )
			{
				LogDebug("%s buff2 get nested error ret %d",__func__, iRet);
			}
		}
		else
		{
			LogDebug("%s buff1 get nested ok ret %d",__func__, iRet);
		}
		return iRet;
	}

	int SKTLVMergePack :: GetBuff( const int &iFieldID, char * &pcValue, int &iSize )
	{
		iSize = 0;
		{
			int iRet = m_ptImpl->poTlvPack2->GetBuf(iFieldID, NULL, &iSize);
			if( iSize > 0 )
			{
				pcValue = (char*)malloc(iSize);
				iRet = m_ptImpl->poTlvPack2->GetBuf(iFieldID, pcValue, &iSize);
				if( iRet )
				{
					LogDebug("%s get buff from buff2 error ret %d",__func__, iRet );
				}
				return iRet;
			}
		}
		{
			int iRet = m_ptImpl->poTlvPack1->GetBuf(iFieldID, NULL, &iSize);
			if( iSize >  0 )
			{
				pcValue = (char*)malloc(iSize);
				iRet = m_ptImpl->poTlvPack1->GetBuf(iFieldID, pcValue, &iSize);
				if( iRet )
				{
					LogDebug("%s get buff from buff1 error ret %d",__func__, iRet );
				}
				return iRet;
			}
		}
		return -1;
	}

	int SKTLVMergePack :: GetBaseTLVPack( const int &iFieldID, CTLVPack ** poTlvPack )
	{
		int iSize = 0;
		{
			m_ptImpl->poTlvPack2->GetBuf(iFieldID, NULL, &iSize);
			if( iSize > 0 )
			{
				*poTlvPack = m_ptImpl->poTlvPack2;
				return 0;
			}
		}
		{
			m_ptImpl->poTlvPack1->GetBuf(iFieldID, NULL, &iSize);
			if( iSize >  0 )
			{
				*poTlvPack = m_ptImpl->poTlvPack1;
				return 0;
			}
		}
		return -1;
	}

};
