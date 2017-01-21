#ifndef _QQMAIL_CROSSPLATFORM_TYPE_XYSSL_H_
#define _QQMAIL_CROSSPLATFORM_TYPE_XYSSL_H_

#ifndef MACOS
#ifndef android
#ifndef linux
#ifndef __SYMBIAN32__
//windows
	// types
	#ifndef _INC_WINDOWS
		#ifndef WIN32_LEAN_AND_MEAN	
			#define WIN32_LEAN_AND_MEAN
		#endif
//		#include <windows.h>
	#endif

	// htonl/ntohl htons/ntohs support
#if 0
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
	#include <e32math.h>
	int rand();
#endif
 
#ifdef android
//android
    #include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <limits.h>
	#include <assert.h>
	#include <math.h>
#endif

#ifdef linux	
//linux
	#include <sys/types.h>
	//#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	//#include <limits.h>
	//#include <assert.h>
	//#include <math.h>

	// htonl/ntohl htons/ntohs support
	//#include <netinet/in.h>
#endif

#ifdef MACOS
	#include <sys/types.h>
	#include <stdlib.h>
	#include <string.h>
#endif

#endif /* _QQMAIL_CROSSPLATFORM_TYPE_XYSSL_H_ */
