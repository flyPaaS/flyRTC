#ifndef BASE_SOCKETDEFINE_H_
#define BASE_SOCKETDEFINE_H_

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#endif  // _WIN32

#if defined(_WIN32)
typedef SOCKET SOCKET_HANDLE;
#define INVALID_SOCKET_HANDLE INVALID_SOCKET
#define SOCKET_ERROR_RETURN SOCKET_ERROR
#define ERROR_NO (WSAGetLastError())
#else
typedef int SOCKET_HANDLE;
#define INVALID_SOCKET_HANDLE -1
#define SOCKET_ERROR_RETURN -1
#define ERROR_NO errno
#endif  // _WIN32

#if !defined(NDEBUG)
#define CHECK_FUNC(val)  \
	do  {  \
		if (val == SOCKET_ERROR_RETURN)  \
			printf("%s(%d) error, errno=%d.\n", __FILE__, __LINE__, val);  \
	} while (0)

#define CHECK_FUNC_RETURN(val)  \
	do {  \
		if (val == SOCKET_ERROR_RETURN) {  \
			printf("%s(%d) error, errno=%d.\n", __FILE__, __LINE__, val);  \
			return;  \
		}  \
	} while (0)

#define CHECK_FUNC_RETURN_VAL(val, ret)  \
	do {  \
		if (val == SOCKET_ERROR_RETURN) {  \
			printf("%s(%d) error, errno=%d.\n", __FILE__, __LINE__, val);  \
			return ret;  \
		}  \
	} while (0)
#else
#define CHECK_FUNC(val)

#define CHECK_FUNC_RETURN(val)  \
	do {  \
		if (val == SOCKET_ERROR_RETURN) {  \
			return;  \
		}  \
	} while (0)

#define CHECK_FUNC_RETURN_VAL(val, ret)  \
	do {  \
		if (val == SOCKET_ERROR_RETURN) {  \
			return ret;  \
		}  \
	} while (0)
#endif  // NDEBUG

// Error define.
#if defined(_WIN32)
#define EINTR WSAEINTR
#define EINPROGRESS WSAEINPROGRESS
#define EWOULDBLOCK WSAEWOULDBLOCK
#endif  // _WIN32

#endif  // BASE_SOCKETDEFINE_H_
