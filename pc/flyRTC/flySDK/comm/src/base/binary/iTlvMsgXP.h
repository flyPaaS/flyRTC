#pragma once

#include "iProtocolXP.h"
#include "iMsgHeadXP.h"
#include "iMsgBodyXP.h"
#include "iTypeXP.h"

namespace Comm
{

class CMsgBodyXP;

#define MODULE_NOT_INITED -1
#define MODULE_INITED     1

/**
	@brief a class for qqmail unique network portocol WITHOUT multi-thread supported
*/
class CNetMsgXP
{
	public:
		CNetMsgXP();
		~CNetMsgXP();

		/**
			@brief API init function
			@retval 0 -- inited ok
			@retval != 0 -- failed
			@note
		*/
		int Init() ;


		/**
			@brief API destroy function
			@note This function released all API resources. After this function is invoked, this class become unavailabe, you need to reinit by invoking Init() again
		*/
		void Destroy() ;


		/**
			@brief API clear function, clear old data to aVOIDe data confused.
			@retval N/A
		*/
		void Clear() ;


		/**
			@brief Pack message INT32o buffer
			@param[out] a_piSize -- packed buffer size
			@retval Msg buffer poINT32er -- ok
			@retval NULL -- failed
			@note If success, the return buffer should be delete[] explicitly.
		*/
		char *PackMsg(int *a_piSize) ;


		/**
			@brief Unpack message
			@param[in] a_pcMsg -- binary data buffer
			@param[in] a_iMsgSize -- data buffer size
			@retval 0 -- ok
			@retval !0 -- failed
		*/
		int UnpackMsg(const char *a_pcMsg, const int a_iMsgSize) ;


		/**
			@brief Get header data process class poINT32er
			@retval CMsgHeadXP poINT32er -- ok
			@retval NULL -- failed
			@note This poINT32er should *NOT* be deleted by external user
		*/
		CMsgHeadXP *GetHeaderPtr() ;


		/**
			@brief Get body data process class poINT32er
			@retval CBaseValList poINT32er -- ok
			@retval NULL -- failed
			@note This poINT32er should *NOT* be deleted by external user
		*/
		CMsgBodyXP *GetBodyPtr() ;


		int CalLen ( ) ;


	private:
		/**
			@brief
			@retval > 0 -- class available
			@retval < 0 -- not available
			@param 
			@note
		*/
		int Check ( ) ;

		void SetByteOrder ( ) ;


	private:
		int m_iInited ; /* -1 -- not inited, 1 -- inited */

		class CMsgHeadXP *m_poMsgHead;
		class CMsgBodyXP *m_poMsgBody;

} ;

}

