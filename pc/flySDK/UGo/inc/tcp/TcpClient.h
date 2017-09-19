#ifndef BASE_TCPCLIENT_H_
#define BASE_TCPCLIENT_H_

#include "socket_platform.h"

// Class TCP client.
class CTcpClient : public CSocketPlatform {
public:
    CTcpClient(bool ipv6Enabled);
    ~CTcpClient();

#if 0
    // Bind server to ip and port.
    // | ip | [in] Server to bind.
    // | port | [in] Port to bind.
    // Return true if bind success.
    bool Bind(const char* ip, int port);
#endif

    // Connect to a server.
    // | ip | [in] the server to connect.
    // | port | [in] the server port.
    // Return true if connected success.
    int Connect(const char* ip, int port);

    // Assign socket value.
    // | sk_handler | [in] socket handler.
    void Assign(SOCKET_HANDLE sk_handler);

public:
	CTcpClient(SOCKET_HANDLE sk);
};

#endif  // BASE_TCPCLIENT_H_

