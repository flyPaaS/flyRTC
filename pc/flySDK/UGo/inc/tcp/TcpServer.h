#ifndef BASE_TCPSERVER_H_
#define BASE_TCPSERVER_H_

#include "socket_platform.h"

class CTcpClient;

// Socket server.
class CTcpServer : public CSocketPlatform {
public:
    CTcpServer();
    ~CTcpServer();

    // Bind server to ip and port.
    // | ip | [in] Server to bind.
    // | port | [in] Port to bind.
    // Return true if bind success.
    bool Bind(int port);

    // Listen socket.
    // | backcnt | [in] the count of back-end socket.
    // Return true if listened success.
    bool Listen(int backcnt);

    // Accept a new socket connect.
    // | pClient | [out] is the socket accepted.
    // Return true if success.
    bool Accept(CTcpClient* pClient);
};

#endif  // BASE_TCPSERVER_H_
