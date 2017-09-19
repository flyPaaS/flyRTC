#include "socket_platform.h"

#include <assert.h>

#if defined(_WIN32)

bool InitSocketClass()
{
    WSAData wsa;
	WORD wVersionRequested;
	wVersionRequested = MAKEWORD(2, 0);
	
	if (WSAStartup(wVersionRequested, &wsa) != 0)
		return false;

    return true;
}

void UninitSocketClass()
{
    WSACleanup();
}

//////////////////////////////////////////////////////////////////////////
// Class CSocketPlatform.
CSocketPlatform::CSocketPlatform(bool isIPv6Enabled)
	: socket_(INVALID_SOCKET_HANDLE),
	isIPv6Enabled_(isIPv6Enabled)
{
	CreateSocket();
	isNonBlock_ = false;
}

CSocketPlatform::~CSocketPlatform()
{
	DestroySocket();
}

void CSocketPlatform::SetNonBlock(bool isNonBlock)
{
	if (socket_ != INVALID_SOCKET_HANDLE) {
		long lVal = isNonBlock ? 1 : 0;
		CHECK_FUNC(::ioctlsocket(socket_, FIONBIO, (u_long*)&lVal));
		isNonBlock_ = isNonBlock;
	}
}

bool CSocketPlatform::IsNonBlock() const
{
	// It's no way to query blocking status on win32.
	return isNonBlock_;
}

void CSocketPlatform::SetRecvBufferSize(int nSize)
{
    if (socket_ != INVALID_SOCKET_HANDLE) {
	    if (nSize < 0)
		    nSize = 0;
	    CHECK_FUNC(
		    ::setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (char*)nSize, sizeof(nSize)));
    }
}

int CSocketPlatform::GetRecvBufferSize() const
{
	int nVal = 0;
    if (socket_ != INVALID_SOCKET_HANDLE) {
	    int nValLen = sizeof(nVal);
	    CHECK_FUNC_RETURN_VAL(
		    ::getsockopt(socket_, SOL_SOCKET, SO_RCVBUF, (char*)&nVal, &nValLen), -1);
    }
    return nVal;
}

void CSocketPlatform::SetSendBufferSize(int nSize)
{
    if (socket_ != INVALID_SOCKET_HANDLE) {
	    if (nSize < 0)
		    nSize = 0;
	    CHECK_FUNC(
		    ::setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (char*)&nSize, sizeof(nSize)));
    }
}

int CSocketPlatform::GetSendBufferSize() const
{
	int nVal = 0;
    if (socket_ != INVALID_SOCKET_HANDLE) {
	    int nValLen = sizeof(nVal);
	    CHECK_FUNC_RETURN_VAL(
		    ::getsockopt(socket_, SOL_SOCKET, SO_SNDBUF, (char*)nVal, &nValLen), -1);
    }
	return nVal;
}

void CSocketPlatform::SetLinger(int nVal)
{
    if (socket_ != INVALID_SOCKET_HANDLE) {
        linger lgr;
        lgr.l_onoff = 1;
        lgr.l_linger = nVal;
	    CHECK_FUNC(
		    ::setsockopt(socket_, SOL_SOCKET, SO_LINGER, (char*)&lgr, sizeof(lgr)));
    }
}

bool CSocketPlatform::IsLinger() const
{
    linger lgr = {0, };
    if (socket_ != INVALID_SOCKET_HANDLE) {
	    int nValLen = sizeof(lgr);
	    CHECK_FUNC_RETURN_VAL(
		    ::getsockopt(socket_, SOL_SOCKET, SO_LINGER, (char*)&lgr, &nValLen), false);
    }
    return lgr.l_onoff != 0;
}

bool CSocketPlatform::IsError() const
{
	int error = -1;
	
    if (socket_ != INVALID_SOCKET_HANDLE) {
	    int nValLen = sizeof(int);
	    CHECK_FUNC_RETURN_VAL(
		    ::getsockopt(socket_, SOL_SOCKET, SO_ERROR, (char*)&error, &nValLen), false);
    }
    return error != 0;
}


void CSocketPlatform::EnableAddressReuse(bool isEnable)
{
    if (socket_ != INVALID_SOCKET_HANDLE) {
	    int nVal = isEnable ? 1 : 0;
	    CHECK_FUNC(
		    ::setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (char*)&nVal, sizeof(nVal)));
    }
}

bool CSocketPlatform::IsAddressReuse() const
{
	int nVal = 0;
    if (socket_ != INVALID_SOCKET_HANDLE) {
	    int nValLen = 0;
	    CHECK_FUNC_RETURN_VAL(
		    ::getsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (char*)&nVal, &nValLen), false);
    }
	return nVal != 0;
}

void CSocketPlatform::EnableNoDelay(bool isEnable)
{
    if (socket_ != INVALID_SOCKET_HANDLE) {
        int nVal = isEnable ? 1 : 0;
        CHECK_FUNC(
            ::setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (char*)&nVal, sizeof(nVal)));
    }
}

bool CSocketPlatform::IsNoDelay() const
{
    int nVal = 0;
    if (socket_ != INVALID_SOCKET_HANDLE) {
        int nValLen = 0;
        CHECK_FUNC_RETURN_VAL(
            ::getsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, (char*)&nVal, &nValLen), false);
    }
    return nVal != 0;
}

int CSocketPlatform::Send(const char* pBuffer, int nLen)
{
    if (socket_ != INVALID_SOCKET_HANDLE) {
        return ::send(socket_, pBuffer, nLen, 0);
    }
    return SOCKET_ERROR_RETURN;
}

int CSocketPlatform::Receive(char* pBuffer, int nLen)
{
    if (socket_ != INVALID_SOCKET_HANDLE) {
        return ::recv(socket_, pBuffer, nLen, 0);
    }
    return SOCKET_ERROR_RETURN;
}

int CSocketPlatform::Select(int nTime, bool CanRead, bool CanWrite)
{
    if (socket_ != INVALID_SOCKET_HANDLE) {
        fd_set p_fs_read;
        fd_set p_fs_write;

        FD_ZERO(&p_fs_read);
        FD_ZERO(&p_fs_write);

        if (CanRead) {            
            FD_SET(socket_, &p_fs_read);
        }
        if (CanWrite) {
            FD_SET(socket_, &p_fs_write);
        }

        timeval* ptv = NULL;
        if (nTime >= 0) {
            ptv = new timeval;
            ptv->tv_sec = nTime / 1000;
            ptv->tv_usec = (nTime % 1000) * 1000;
        }

        int nSelRet = ::select(socket_ + 1, &p_fs_read, &p_fs_write, NULL, ptv);
        if (nSelRet > 0) {
            nSelRet = 0;
            if (CanRead) {
                nSelRet += FD_ISSET(socket_, &p_fs_read) ? 1 : 0;
            }
            if (CanWrite) {
                nSelRet += FD_ISSET(socket_, &p_fs_write) ? 1 : 0;
            }
        }
        delete ptv;
        return nSelRet;
    }
    return -1;
}

CSocketPlatform::CSocketPlatform(SOCKET_HANDLE sk)
    : socket_(sk)
{
    assert(sk != INVALID_SOCKET_HANDLE);
}

bool CSocketPlatform::CreateSocket()
{
    int af = AF_INET;
	if (isIPv6Enabled_)
	{
		af = AF_INET6;
	}
    socket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	CHECK_FUNC(socket_);
	return socket_ != INVALID_SOCKET_HANDLE;
}

void CSocketPlatform::DestroySocket()
{
    if (socket_ != INVALID_SOCKET_HANDLE) {
        ::closesocket(socket_);
        socket_ = INVALID_SOCKET_HANDLE;
    }
}

#endif  // _WIN32
