#ifndef BASE_SOCKETPLATFORM_H_
#define BASE_SOCKETPLATFORM_H_

#include "socket_define.h"

#include <stdio.h>

// Initialize socket class.
// Return true if success.
bool InitSocketClass();

// Un-initialize socket class.
void UninitSocketClass();

// Class CSocketPlatform, wrap platform independent socket.
class CSocketPlatform {
public:
	CSocketPlatform(bool isIPv6Enabled = false);
	~CSocketPlatform();

	//getsockopt SO_ERROR
	bool IsError() const;

	// Set blocking or not.
	// | isNonBlock | [in] true to set blocking.
	void SetNonBlock(bool isNonBlock);
	// Check whether blocking or not.
	// Return true if socket is blocking.
	bool IsNonBlock() const;

	// Set receive buffer size.
	// | nSize | [in] is the size(bytes) to set.
	void SetRecvBufferSize(int nSize);
	// Get receive buffer size.
	// Return current buffer size if success, otherwise return -1.
	int GetRecvBufferSize() const;

	// Set send buffer size.
	// | nSize | [in] is the size(bytes) to set.
	void SetSendBufferSize(int nSize);
	// Get send buffer size.
	// Return current send buffer size if success, otherwise return -1.
	int GetSendBufferSize() const;

	// Set socket linger value.
	// | nVal | [in] the socket linger to set.
	void SetLinger(int nVal);
	// Get linger status.
	// Return true if linger enabled.
	bool IsLinger() const;

	// Enable or disable address reuse.
	// | isEnable | [in] true to enable address reuse.
	void EnableAddressReuse(bool isEnable);
	// Get address reuse status.
	// Return true if address reuse enabled.
	bool IsAddressReuse() const;

    // Enable Nagle algorithm or not.
    // | isEnable | [in] true to enable Nagle algorithm.
    void EnableNoDelay(bool isEnable);
    // Get Nagle algorithm status.
    // Return true if Nagle algorithm enabled.
    bool IsNoDelay() const;

    // Send data by socket.
    // | pBuffer | [out] Content to write.
    // | nLen | [in] Buffer length.
    // Return count sent if success, otherwise return -1.
    int Send(const char* pBuffer, int nLen);

    // Receive data from socket.
    // | pBuffer | [in] Content to write.
    // | nLen | [in] Buffer length.
    // Return received count if success, otherwise return -1.
    int Receive(char* pBuffer, int nLen);

    // Select socket read or write.
    // | nTime | [in] Select time(ms). -1 for select infinitely.
    // | CanRead | [in] Select read.
    // | CanWrite | [in] Select write.
    // Return -1 if error, 0 for timeout, >0 for socket can read or write.
    int Select(int nTime, bool CanRead, bool CanWrite);

protected:
    CSocketPlatform(SOCKET_HANDLE sk);

private:
	bool CreateSocket();
	void DestroySocket();

protected:
	SOCKET_HANDLE socket_;
	bool	isNonBlock_;
	bool    isIPv6Enabled_;
};

#endif  // BASE_SOCKETPLATFORM_H_
