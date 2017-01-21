#include <string.h>

#include "iTlvMsgXP.h"

namespace Comm
{

/* class CNetMsgXP */
CNetMsgXP::CNetMsgXP()
{
	m_iInited = MODULE_NOT_INITED ;

	m_poMsgHead = NULL ;
	m_poMsgBody = NULL ;
}


CNetMsgXP::~CNetMsgXP()
{
	Destroy();
}


int CNetMsgXP::Init()
{
	int iRet = NMXPRC_OK ;

	if (MODULE_INITED == m_iInited)
	{
		return iRet ;
	}

	m_poMsgHead = new CMsgHeadXP();
	if (NULL == m_poMsgHead)
	{
		iRet = NMXPRC_ERR_NOMEM ;
	}
	else
	{
		m_poMsgHead->Clear() ;
	}

	m_poMsgBody = new CMsgBodyXP();
	if (NULL == m_poMsgBody)
	{
		iRet = NMXPRC_ERR_NOMEM;
	}
	
	if (NMXPRC_OK == iRet)
	{
		m_iInited = MODULE_INITED ;
	}
	else
	{
		this->Destroy() ;
	}

	return iRet ;
}


void CNetMsgXP::Destroy()
{
	m_iInited = MODULE_NOT_INITED ;

	if (NULL != m_poMsgHead)
	{
		m_poMsgHead->Clear();
		delete m_poMsgHead;
	}
	m_poMsgHead = NULL;

	if (NULL != m_poMsgBody)
	{
		delete m_poMsgBody;
	}
	m_poMsgBody = NULL;
}


void CNetMsgXP::Clear()
{
	int iRet = 0 ;
	iRet = Check() ;
	if ( 0 > iRet )
	{
		return ;
	}

	m_poMsgHead->Clear();
	m_poMsgBody->Clear();
	return ;
}


int CNetMsgXP::Check()
{
	int iRet = MODULE_NOT_INITED;

	if ((MODULE_INITED == m_iInited) && 
		(NULL != m_poMsgHead) && 
		(NULL != m_poMsgBody))
	{
		iRet = MODULE_INITED;
	}

	return iRet;
}


CMsgHeadXP *CNetMsgXP::GetHeaderPtr()
{
	return m_poMsgHead;
}


CMsgBodyXP *CNetMsgXP::GetBodyPtr()
{
	return m_poMsgBody;
}

char *CNetMsgXP::PackMsg(int *a_piSize)
{
	int iRet = 0;
	int iHeadSize = 0;
	int iBodySize = 0;
	int iMsgSize = 0;
	char *pcHead = NULL;
	char *pcBody = NULL;
	char *pcMsg = NULL;

	if (NULL != a_piSize)
	{
		memset((char*)a_piSize, 0, sizeof(int));
	}

	iRet = Check();
	if (0 > iRet)
	{
		return NULL ;
	}

	/* pack body */
	{
		iRet = this->GetBodyPtr()->PackToBuffer(&pcBody, &iBodySize) ;
		if (0 != iRet)
		{
			/* failed, pcBody would be delete before return */
		}
		else
		{
			if ((NULL == pcBody) || (0 >= iBodySize))
			{
				iRet = NMXPRC_ERR_GENERAL;
			}
		}
	}

	/* pack message */
	if (NMXPRC_OK == iRet)
	{
		this->GetHeaderPtr()->SetBodyLen(iBodySize) ;

		pcHead = this->GetHeaderPtr()->PackHead(&iHeadSize) ;
		if ((NULL == pcHead) || (0 >= iHeadSize))
		{
			/* failed, pcHead would be delete before return */
			iRet = NMXPRC_ERR_GENERAL ;
		}
		else
		{
			iMsgSize = iHeadSize + iBodySize ;

			pcMsg = new char[iMsgSize] ;
			if (NULL == pcMsg)
			{
				iRet = NMXPRC_ERR_NOMEM ;
			}
			else
			{
				memcpy(pcMsg, pcHead, iHeadSize);
				memcpy(pcMsg + iHeadSize, pcBody, iBodySize);
			}
		}
	}

	/* clear */
	if (NULL != pcBody)
	{
		delete[] pcBody ;
	}
	pcBody = NULL ;

	if (NULL != pcHead)
	{
		delete[] pcHead ;
	}
	pcHead = NULL;

	if (NULL != a_piSize)
	{
		memcpy(a_piSize, &iMsgSize, sizeof(int));
	}

	return pcMsg ;
}

int CNetMsgXP::UnpackMsg(const char *a_pcMsg, const int a_iMsgSize)
{
	int iRet = NMXPRC_OK;
	int iHeadSize = 0;
	int iBodySize = 0;

	if ((NULL == a_pcMsg) || ((int)sizeof(QQMAILMSGHEADXP) > a_iMsgSize))
	{
		return NMXPRC_ERR_INVALID_ARG;
	}

	iRet = Check();
	if (0 > iRet)
	{
		return NMXPRC_ERR_GENERAL;
	}

	iRet = this->GetHeaderPtr()->UnpackHead(a_pcMsg);
	if (NMXPRC_OK == iRet)
	{
		iHeadSize = this->GetHeaderPtr()->GetHeadLen();
		iBodySize = this->GetHeaderPtr()->GetBodyLen();

		if (a_iMsgSize < iBodySize)
		{
			return NMXPRC_ERR_INVALID_ARG;
		}

		this->GetBodyPtr()->Clear();

		iRet = this->GetBodyPtr()->InitFromBuffer(a_pcMsg + iHeadSize, iBodySize);
		if (NMXPRC_OK == iRet)
		{
			/* ok */
		}
	}

	return iRet ;
}

int CNetMsgXP::CalLen()
{
	return (this->GetHeaderPtr()->GetHeadLen() + this->GetHeaderPtr()->GetBodyLen());
}

}

