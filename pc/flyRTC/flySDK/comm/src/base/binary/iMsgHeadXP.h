#pragma once

#include "iProtocolXP.h"
#include "iTypeXP.h"

namespace Comm
{

/**
	@brief a class for qqmail unique network portocol without multi-thread supported
*/
class CMsgHeadXP
{
	public:
		CMsgHeadXP() ;
		~CMsgHeadXP() ;

		void Clear ( ) ;

		/* ********* SetXXX() function set begin ********* */
		void SetMagic ( const unsigned short a_hMagic ) ;

		void SetVersion ( const unsigned char a_bVersion ) ;

		void SetCmdID ( const unsigned short a_hCmdID ) ;

		void SetBodyLen ( const unsigned int a_iBodyLen ) ;

		void SetSequence ( const unsigned int a_iSeq ) ;

		void SetUin ( const unsigned int a_iUin ) ;

		void SetResult ( const unsigned short a_hResult ) ;

		void SetReserved ( const unsigned char *a_pcReserved, const int a_iLen );
		/* ********* SetXXX() function set end ********* */

		/* ********* GetXXX() function set begin ********* */
		int GetMagic ( ) ;

		int GetVersion ( ) ;

		int GetCmdID ( ) ;

		int GetBodyLen ( ) ;

		int GetHeadLen ( ) ;

		unsigned int GetSequence ( ) ;

		int GetByteOrder ( ) ;

		unsigned int GetUin ( ) ;

		unsigned short GetResult ( ) ;

		const unsigned char *GetReserved ( int *a_iLen ) ;
		/* ********* GetXXX() function set end ********* */

		char *PackHead ( int *a_piSize ) ;

		int UnpackHead ( const char *a_pcBuffer ) ;

	private:
		unsigned short CheckSum ( const unsigned char *a_pcBuffer, const int a_iBufLen ) ;


	private:
		Comm::QQMAILMSGHEADXP m_tMsgHead ;

		char m_sBuffer[128] ;
} ;

}

