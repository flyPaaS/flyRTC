#include "TcpServer.h"
#include "TcpClient.h"

#if defined(_WIN32)
typedef int socklen_t;
#endif

#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#if defined (ANDROID)
#include <linux/tcp.h>
#else
#include <netinet/tcp.h>
#endif
#include <arpa/inet.h>
#endif
//////////////////////////////////////////////////////////////////////////
// Class CSocketServer.
CTcpServer::CTcpServer()
    : CSocketPlatform()
{
}

CTcpServer::~CTcpServer()
{
}

bool CTcpServer::Bind(int port)
{
    if (socket_ != INVALID_SOCKET_HANDLE) {
        sockaddr_in add_svr;
        add_svr.sin_family = AF_INET;
        add_svr.sin_addr.s_addr = htons(INADDR_ANY);
        add_svr.sin_port = htons(port);
        CHECK_FUNC_RETURN_VAL(
            ::bind(socket_, (sockaddr*)&add_svr, sizeof(add_svr)), false);
        return true;
    }
    return false;
}

bool CTcpServer::Listen(int backcnt)
{
    if (socket_ != INVALID_SOCKET_HANDLE) {
        CHECK_FUNC_RETURN_VAL(
            ::listen(socket_, backcnt < 0 ? 0 : backcnt), false);
        return true;
    }
    return false;
}

bool CTcpServer::Accept(CTcpClient* pClient)
{
    if (socket_ != INVALID_SOCKET_HANDLE && pClient != NULL) {
        sockaddr_in add_client;
        int add_len = sizeof(add_client);
        SOCKET_HANDLE sck_client = ::accept(socket_, (sockaddr*)&add_client,(socklen_t*) &add_len);
        if (sck_client != INVALID_SOCKET_HANDLE) {
            printf("IP[%s] port[%d] connected.\n", inet_ntoa(add_client.sin_addr), ntohs(add_client.sin_port));
            pClient->Assign(sck_client);
            return true;
        }
    }
    return false;
}
