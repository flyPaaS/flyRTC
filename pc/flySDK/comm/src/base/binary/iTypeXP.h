#pragma once

#ifdef WIN32
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

#ifdef linux	

	#include <sys/types.h>

	// htonl/ntohl htons/ntohs support
	#include <netinet/in.h>
#endif

