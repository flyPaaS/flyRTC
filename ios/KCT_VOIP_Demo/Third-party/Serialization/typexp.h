#ifndef _QQMAIL_CROSSPLATFORM_TYPE_H_
#define _QQMAIL_CROSSPLATFORM_TYPE_H_

#ifndef MACOS
#ifndef android
#ifndef linux
#ifndef __SYMBIAN32__
//windows

#if 0

	// types
	#ifndef _INC_WINDOWS
		#ifndef WIN32_LEAN_AND_MEAN	
			#define WIN32_LEAN_AND_MEAN
		#endif
		#include <windows.h>
	#endif

	// htonl/ntohl htons/ntohs support
	#include <WinSock2.h>
	#ifdef _WIN32_WCE
		#pragma comment(lib, "Ws2.lib")
	#else
		#pragma comment(lib, "Ws2_32.lib")
	#endif

#endif


#endif
#endif
#endif
#endif

#ifdef __SYMBIAN32__
//symbain
	#include <e32std.h>
	#include <e32base.h>	
	#include<ezcompressor.h>
	#include<ezdecompressor.h>

	#define htons(n) ((((n) & 0xFFU) << 8) | (((n) & 0xFF00U) >> 8))
	#define ntohs(n) ((((n) & 0xFFU) << 8) | (((n) & 0xFF00U) >> 8))
	#define htonl(n) ((((n) & 0xFFU) << 24) | (((n) & 0xFF00U) << 8) | (((n) & 0xFF0000U) >> 8) | (((n)& 0xFF000000U) >> 24))
	#define ntohl(n) ((((n) & 0xFFU) << 24) | (((n) & 0xFF00U) << 8) | (((n) & 0xFF0000U) >> 8) | (((n)& 0xFF000000U) >> 24))
	#define INT_MAX 0x7FFFFFFF
    extern "C" int atoi(const char *aps);
#endif
 
#ifdef android
//android
    #include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <limits.h>
	#include <assert.h>
	#include <zlib.h>
	#define htons(n) ((((n) & 0xFFU) << 8) | (((n) & 0xFF00U) >> 8))
	#define ntohs(n) ((((n) & 0xFFU) << 8) | (((n) & 0xFF00U) >> 8))
	#define htonl(n) ((((n) & 0xFFU) << 24) | (((n) & 0xFF00U) << 8) | (((n) & 0xFF0000U) >> 8) | (((n)& 0xFF000000U) >> 24))
	#define ntohl(n) ((((n) & 0xFFU) << 24) | (((n) & 0xFF00U) << 8) | (((n) & 0xFF0000U) >> 8) | (((n)& 0xFF000000U) >> 24))
    int memcompare (const unsigned char* aLeft, int aLeftLen, const unsigned char* aRight, int aRightLen);
#endif

#ifdef linux	
//linux
	#include <sys/types.h>
	#include <stdlib.h>
	#include <string.h>

	// htonl/ntohl htons/ntohs support
	#include <netinet/in.h>
#endif

#ifdef MACOS
	#include <sys/types.h>
	#include <stdlib.h>
	#include <string.h>
#endif

#endif /* _QQMAIL_CROSSPLATFORM_TYPE_H_ */
