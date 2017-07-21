#include "TcpClient.h"

#include <assert.h>

#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#if defined (ANDROID)
#include <linux/tcp.h>
#else

#endif
#include <arpa/inet.h>
#endif
//////////////////////////////////////////////////////////////////////////
int InetPresentationToNumeric(int af,
                                        const char* src,
                                        void* dst)
{
#if defined(_WIN32)
	sockaddr temp;
	int length = sizeof(sockaddr);

    if(af == AF_INET)
    {
        int result = WSAStringToAddressA(
            (const LPSTR)src,
            af,
            0,
            reinterpret_cast<struct sockaddr*>(&temp),
            &length);
        if(result != 0)
        {
            return -1;
        }
        memcpy(dst, &((struct sockaddr_in*)&temp)->sin_addr,
			sizeof(((struct sockaddr_in*)&temp)->sin_addr));
        return 0;
    }
    else if(af == AF_INET6)
    {
        int result = WSAStringToAddressA(
            (const LPSTR)src,
            af,
            0,
            reinterpret_cast<struct sockaddr*>(&temp),
            &length);
        if(result !=0)
        {
            return -1;
        }
        memcpy(dst,&((struct sockaddr_in6*)&temp)->sin6_addr,
			sizeof(((struct sockaddr_in6*)&temp)->sin6_addr));
        return 0;

    }else
    {
        return -1;
    }    
#else
    const int result = inet_pton(af, src, dst);
    return result > 0 ? 0 : -1;
#endif
}

// Class CTcpClient.
CTcpClient::CTcpClient(bool ipv6Enabled)
    : CSocketPlatform(ipv6Enabled)
{
}

CTcpClient::~CTcpClient()
{
}

int CTcpClient::Connect(const char* ip, int port)
{
	int res = SOCKET_ERROR_RETURN;

    if (socket_ != INVALID_SOCKET_HANDLE) {
        if ( isIPv6Enabled_ )
        {
            struct sockaddr_in6 add_server;
            add_server.sin6_family = AF_INET6;
            InetPresentationToNumeric(AF_INET6, ip, 
                &add_server.sin6_addr);
            add_server.sin6_port = htons(port);
            res = ::connect(socket_, (sockaddr*)&add_server, sizeof(add_server));
        }
        else
        {
            struct sockaddr_in add_server;
            add_server.sin_family = AF_INET;
            InetPresentationToNumeric(AF_INET, ip, 
                &add_server.sin_addr);
            add_server.sin_port = htons(port);
            res = ::connect(socket_, (sockaddr*)&add_server, sizeof(add_server));
        }
		if (res < 0 && IsNonBlock())
		{
			res = 1;
		}  
    }
		
    return res;
}

void CTcpClient::Assign(SOCKET_HANDLE sk_handler)
{
    socket_ = sk_handler;
    assert(socket_ != INVALID_SOCKET_HANDLE);
}


CTcpClient::CTcpClient(SOCKET_HANDLE sk)
	: CSocketPlatform(sk)
{
}


