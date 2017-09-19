/***********************************************************************
 *
 * File:        Socket.cpp
 * Author:      iotazhang
 * Revision:    $Id: MmSocket.cpp 376038 2012-12-05 07:48:00Z iotazhang $
 * Description: socket操作
 *
 ***********************************************************************/

#include "MmSocket.h"
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <iostream>
#include <sys/socket.h>
#include <netinet/tcp.h>

#include <stdio.h>

namespace mm {

using std::min;

///////////////////////////////////////////////////////////SocketAddress

SocketAddress::SocketAddress() : _ip(htonl(INADDR_NONE)), _port(0) {}

SocketAddress::SocketAddress(unsigned short port) {
    setAddress(port);
}

SocketAddress::SocketAddress(const string& addr) {
    setAddress(addr);
}

SocketAddress::SocketAddress(const string& addr, unsigned short port) {
    setAddress(addr, port);
}

SocketAddress::SocketAddress(const sockaddr_in& addr) {
    setAddress(addr);
}

void SocketAddress::setAddress(unsigned short port) {
    _ip = htonl(INADDR_ANY);
    _port = port;
}

void SocketAddress::setAddress(const string& addr) {
    string::size_type pos = addr.find_last_of(":");
    if (pos == string::npos || pos == addr.size() - 1) {
        setAddress(addr, 0);
    } else {
        string port = addr.substr(pos + 1);
        setAddress(addr.substr(0, pos), (unsigned short)atoi(port.c_str()));
    }
}

void SocketAddress::setAddress(const string& addr, unsigned short port) {
    _ip = inet_addr(addr.c_str());
    if (_ip == static_cast<unsigned long>(INADDR_NONE)) {
        throw SocketException("inet_addr error \"" + addr + "\"");
    }
    _port = port;
}

unsigned long SocketAddress::getIp() const {
    return _ip;
}

unsigned short SocketAddress::getPort() const {
    return _port;
}

void SocketAddress::getAddress(sockaddr_in& addr) const {
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = _ip;
    memset(&(addr.sin_zero), 0, 8);
}

void SocketAddress::setAddress(const sockaddr_in& addr) {
    _ip = addr.sin_addr.s_addr;
    _port = ntohs(addr.sin_port);
}

string SocketAddress::getHost() const {
    char buff[16];

    in_addr addr;
    addr.s_addr = _ip;
    if (inet_ntop(AF_INET, &addr, buff, sizeof(buff)) == 0) {
        return string();
    } else {
        return buff;
    }
}

string SocketAddress::toString() const {
    return getHost() + ":" + str(_port);
}

bool SocketAddress::operator ==(const SocketAddress& addr) const {
    return _ip == addr._ip && _port == addr._port;
}

///////////////////////////////////////////////////////////SocketBase

SocketBase::SocketBase() : _handle(-1) {
    initHandle();
}

SocketBase::SocketBase(int handle) : _handle(handle) {}

SocketBase::~SocketBase() {
    if (_handle != -1) {
        ::close(_handle);
        _handle = -1;
    }
}

void SocketBase::initHandle() {
    // 创建socket句柄
    _handle = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_handle == -1) {
        throw SocketException("socket error");
    }
}

int SocketBase::getSocketHandle() const {
    return _handle;
}

void SocketBase::setSocketHandle(int handle) {
    if (_handle != handle) {
        close();
        _handle = handle;
    }
}

int SocketBase::detachSocketHandle() {
    int handle = _handle;
    _handle = -1;
    return handle;
}

bool SocketBase::getNonBlocking() const {
    return getNonBlocking(_handle);
}

void SocketBase::setNonBlocking(bool on) {
    setNonBlocking(_handle, on);
}

bool SocketBase::getNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return flags & O_NONBLOCK;
}

void SocketBase::setNonBlocking(int fd, bool on) {
    int flags = ::fcntl(fd, F_GETFL, 0);

    if (on) {
        if (flags & O_NONBLOCK) {
            return;
        }
        flags |= O_NONBLOCK;
    } else {
        if (!(flags & O_NONBLOCK)) {
            return;
        }
        flags &= ~O_NONBLOCK;
    }

    ::fcntl(fd, F_SETFL, flags);
}

socklen_t SocketBase::getOption(int level, int option, void* value, socklen_t optLen) const {
    if (::getsockopt(_handle, level, option, static_cast<char*>(value), &optLen) == -1) {
        throw SocketException("getsockopt error");
    }
    return optLen;
}

void SocketBase::setOption(int level, int option, void* value, socklen_t optLen) const {
    if (::setsockopt(_handle, level, option, static_cast<char*>(value), optLen) == -1) {
        throw SocketException("setsockopt error");
    }
}

void SocketBase::close() {
    if (_handle != -1) {
        if (::close(_handle) == -1) {
            _handle = -1;
            throw SocketException("close error");
        } else {
            _handle = -1;
        }
    }
}

void SocketBase::reset() {
	close();
	initHandle();
}

///////////////////////////////////////////////////////////Socket

const static int SOCKET_BUF_SIZE = 512;

Socket::Socket() {}

Socket::Socket(const SocketAddress& addr) {
    connect(addr);
}

Socket::Socket(int handle) : SocketBase(handle) {}

Socket::~Socket() {}

int Socket::getSendTimeout() const {
	timeval tv;
	getOption(SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(timeval));
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void Socket::setSendTimeout(int timeout) {
    if (timeout >= 0) {
        timeval tv;
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
        setOption(SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    } else {
        setOption(SOL_SOCKET, SO_SNDTIMEO, 0, sizeof(int));
    }
}

int Socket::getReceiveTimeout() const {
    timeval tv;
    getOption(SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(timeval));
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
	
void Socket::setReceiveTimeout(int timeout) {
    if (timeout >= 0) {
        timeval tv;
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = (timeout % 1000) * 1000;
        setOption(SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    } else {
        setOption(SOL_SOCKET, SO_RCVTIMEO, 0, sizeof(int));
    }
}

void Socket::setSendBufferSize(int size) {
    setOption(SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
}

void Socket::setReceiveBufferSize(int size) {
    setOption(SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
}

#if 0
void Socket::setQuickAck(bool on) {
    int v = on ? 1 : 0;
    setOption(IPPROTO_TCP, TCP_QUICKACK, &v, sizeof(v));
}
#endif

SocketAddress Socket::getRemoteAddress() const {
    return getRemoteAddress(_handle);
}
    
SocketAddress Socket::getRemoteAddress(int fd) {
    // 获取对端ip
    sockaddr_in addr;
    socklen_t len = sizeof(addr);

    mm::SocketAddress remoteAddr;
    if (::getpeername(fd, reinterpret_cast<struct sockaddr*>(&addr), &len) == 0) {
        remoteAddr.setAddress(addr);
    }

    return remoteAddr;
}

SocketAddress Socket::getLocalAddress() const {
    return getLocalAddress(_handle);
}

SocketAddress Socket::getLocalAddress(int fd) {
    // 获取本端ip
    sockaddr_in addr;
    socklen_t len = sizeof(addr);

    mm::SocketAddress localAddr;
    if (::getsockname(fd, reinterpret_cast<struct sockaddr*>(&addr), &len) == 0) {
        localAddr.setAddress(addr);
    }

    return localAddr;
}

void Socket::connect(const SocketAddress& addr) {
    // 准备句柄
    if (_handle == -1) {
        initHandle();
    }

    // 设置地址
    sockaddr_in sockAddr;
    addr.getAddress(sockAddr);

    // 连接目标机器
    if (::connect(_handle, (const sockaddr*)&sockAddr, sizeof(sockAddr)) == -1) {
        if (errno != EINPROGRESS) {
            string msg = "connect " + addr.toString() + " error";
            throw SocketException(msg);
        } else if (!getNonBlocking()) {
            string msg = "connect " + addr.toString() + " timeout";
            throw SocketException(msg);
        }
    }
}

int Socket::send(const char* data, int dataSize, bool* again) {
    const char* p = data;
    int n = 0;

    while (dataSize > 0) {
        n = ::send(_handle, p, dataSize, 0);
        if (n > 0) {
            p += n;
            dataSize -= n;
        } else if (errno == EAGAIN) { // 阻塞或超时
            if (!getNonBlocking()) {
                throw SocketException("send timeout");
            }
            break;
        } else if (errno == EINTR) { // 被中断
            continue;
        } else {
        	printf("send error, errno(%d), %d\n", errno, dataSize);
            throw SocketException("send error");
        }
    }

    if (again) {
        *again = (errno == EAGAIN);
    }

    return p - data;
}

int Socket::send(ByteBuffer& data, bool* again) {
    int n = send(data.begin(), data.remaining(), again);
    data.setPosition(data.getPosition() + n);
    return n;
}

int Socket::receive(char* buffer, int bufferSize, bool* again) {
    char* p = buffer;
    int n = 0;
    while (bufferSize > 0) {
        n = ::recv(_handle, p, bufferSize, 0);
        if (n > 0) {
            p += n;

            //读取完毕
            if (n < bufferSize) {
                break;
            }

            bufferSize -= n;
        } else if (n == 0) { // 连接关闭
            break;
        } else if (errno == EAGAIN) { // 读不到
            if (!getNonBlocking()) {
                throw SocketException("recv timeout");
            }
            break;
        } else if (errno == EINTR) {
            continue;
        } else {
            throw SocketException("recv error");
        }
    }

    if (again) {
        *again = (errno == EAGAIN);
    }

    return p - buffer;
}

int Socket::receive(ByteBuffer& buffer, bool* again) {
    char temp[SOCKET_BUF_SIZE];
    int n = 0, total = 0;
    while ((n = receive(temp, SOCKET_BUF_SIZE, again)) > 0) {
        buffer.write(temp, n);
        total += n;

        //读取完毕则退出
        if (n < SOCKET_BUF_SIZE) {
            break;
        }
    }

    return total;
}

int Socket::receive(ByteBuffer& buffer, int maxBytes, bool* again) {
    char temp[SOCKET_BUF_SIZE];
    int n = 0, remaingBytes = maxBytes;

    //读完指定字节数后才退出
    while (remaingBytes > 0) {
        n = receive(temp, min(SOCKET_BUF_SIZE, remaingBytes), again);
        if (n == 0) {
            break;
        }

        buffer.write(temp, n);
        remaingBytes -= n;
    }

    return maxBytes - remaingBytes;
}

void Socket::shutdownInput() {
    if (::shutdown(_handle, SHUT_RD) == -1) {
        throw SocketException("shutdown(SHUT_RD) error");
    }
}

void Socket::shutdownOutput() {
    if (::shutdown(_handle, SHUT_WR) == -1) {
        throw SocketException("shutdown(SHUT_WR) error");
    }
}

void Socket::shutdown() {
    if (::shutdown(_handle, SHUT_RDWR) == -1) {
        throw SocketException("shutdown error");
    }
}

///////////////////////////////////////////////////////////ServerSocket

ServerSocket::ServerSocket() {}

ServerSocket::ServerSocket(const SocketAddress& addr) {
    listen(addr);
}

ServerSocket::~ServerSocket() {}

void ServerSocket::listen(const SocketAddress& addr) {
    //绑定到本机端口
    struct sockaddr_in localAddr;
    addr.getAddress(localAddr);

    //重用地址
    int reuse = 1;
    setOption(SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    if (::bind(_handle, reinterpret_cast<struct sockaddr*>(&localAddr), sizeof(localAddr)) == -1) {
        throw SocketException("bind error");
    }

    if (::listen(_handle, SOMAXCONN) == -1) {
        throw SocketException("listen error");
    }
}

int ServerSocket::getAcceptTimeout() const {
    timeval tv;
    getOption(SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(timeval));
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void ServerSocket::setAcceptTimeout(int timeout) {
    timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    setOption(SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
}

Socket* ServerSocket::accept() {
    int fd = acceptfd(NULL);
    if (fd >= 0) {
        return new Socket(fd);
    } else {
        return NULL;
    }
}

int ServerSocket::acceptfd(SocketAddress* addr) {
    sockaddr_in a;
    socklen_t n = sizeof(a);

    int fd = ::accept(_handle, reinterpret_cast<struct sockaddr*>(&a), &n);
    if (fd == -1 && (errno != EAGAIN && errno != EWOULDBLOCK)) {
        throw SocketException("accept error");
    } else if (fd >= 0 && addr) {
        addr->setAddress(a);
    }
    return fd;
}

}


