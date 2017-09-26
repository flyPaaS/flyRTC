/***********************************************************************
 *
 * File:        Socket.h
 * Author:      iotazhang
 * Revision:    $Id: MmSocket.h 376038 2012-12-05 07:48:00Z iotazhang $
 * Description: socket操作
 *
 ***********************************************************************/

#ifndef MM_PROXY_SOCKET_H
#define MM_PROXY_SOCKET_H

#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include "MmUtil.h"
#include "MmByteBuffer.h"

namespace mm {

using std::string;

//////////////////////////////////////////////////////////////////SocketAddress

/**
 * Socket地址类
 */
class SocketAddress {
public:
    /**
     * 地址类型
     */
    enum Type {
        TYPE_LOOPBACK = 1, /**< 环回地址 */
        TYPE_INNER = 2, /**< 内网地址 */
        TYPE_OUTER = 3 /**< 外网地址 */
    };

    /**
     * 构造函数
     */
    SocketAddress();

    /**
     * 构造函数
     * @param port 端口
     */
    SocketAddress(unsigned short port);

    /**
     * 构造函数
     * @param addr 地址
     */
    SocketAddress(const string& addr);

    /**
     * 构造函数
     * @param addr 地址
     * @param port 端口
     */
    SocketAddress(const string& addr, unsigned short port);

    /**
     * 设置地址
     * @param addr 地址
     */
    SocketAddress(const sockaddr_in& addr);

    /**
     * 设置地址
     * @param port 端口
     */
    void setAddress(unsigned short port);

    /**
     * 设置地址
     * @param addr 地址
     */
    void setAddress(const string& addr);

    /**
     * 设置地址
     * @param addr 地址
     * @param port 端口
     */
    void setAddress(const string& addr, unsigned short port);

    /**
     * 获取ip, 网络字节序
     */
    unsigned long getIp() const;

    /**
     * 获取端口, 主机字节序
     */
    unsigned short getPort() const;

    /**
     * 获取地址
     * @param addr 地址, 输出参数
     */
    void getAddress(sockaddr_in& addr) const;

    /**
     * 设置地址
     * @param addr 地址
     */
    void setAddress(const sockaddr_in& addr);

    /**
     * 获取主机名称
     */
    string getHost() const;

    /**
     * 转为字符串
     */
    string toString() const;

    /**
     * 获取地址类型
     */
    Type getAddressType() const;

    /**
     * 相等比较
     */
    bool operator ==(const SocketAddress& addr) const;

    /**
     * 获取地址类型
     */
    static Type getAddressType(const string& ip);

private:
    unsigned long   _ip;
    unsigned short  _port;
};

///////////////////////////////////////////////////////////////////////Socket

/**
 * Socket 基类
 */
class SocketBase {
public:
    /**
     * 构造函数
     */
    SocketBase();

    /**
     * 构造函数
     */
    SocketBase(int handle);

    /**
     * 析构函数
     */
    virtual ~SocketBase();

    /**
     * 获取Socket句柄
     */
    virtual int getSocketHandle() const;

    /**
     * 设置Socket句柄
     */
    virtual void setSocketHandle(int handle);

    /**
     * detach 句柄
     */
    virtual int detachSocketHandle();

    /**
     * 获取是否非阻塞
     */
    virtual bool getNonBlocking() const;

    /**
     * 设置非阻塞模式
     */
    virtual void setNonBlocking(bool on);

    /**
     * 设置非阻塞模式
     */
    static void setNonBlocking(int fd, bool on);

    /**
     * 获取是否非阻塞
     */
    static bool getNonBlocking(int fd);

    /**
     * 关闭Socket
     */
    virtual void close();

    /**
     * 重置socket
     */
    virtual void reset();

protected:

    /**
     * 初始化句柄
     */
    void initHandle();

    /**
     * 获取选项
     */
    socklen_t getOption(int level, int option, void* value, socklen_t optLen) const;

    /**
     * 设置选项
     */
    void setOption(int level, int option, void* value, socklen_t optLen) const;

    int _handle;
};

/**
 * Socket类
 */
class Socket : public SocketBase {
public:
    /**
     * 构造函数
     */
    Socket();

    /**
     * 构造函数
     * @param addr 连接地址
     */
    Socket(const SocketAddress& addr);

    /**
     * 构造函数
     */
    Socket(int handle);

    /**
     * 析构函数
     */
    virtual ~Socket();

    /**
     * 获取发送超时/连接超时
     */
    int getSendTimeout() const;

    /**
     * 设置发送超时/连接超时
     * @param timeout 超时毫秒数
     */
    void setSendTimeout(int timeout);

    /**
     * 获取接收超时
     */
    int getReceiveTimeout() const;

    /**
     * 设置接收超时
     */
    void setReceiveTimeout(int timeout);

    /**
     * 设置发送缓冲区
     */
    void setSendBufferSize(int size);

    /**
     * 设置接收缓冲区
     */
    void setReceiveBufferSize(int size);

    /**
     * 设置 quick ack
     */
    void setQuickAck(bool on);

    /**
     * 获取对端地址
     */
    SocketAddress getRemoteAddress() const;

    /**
     * 获取对端地址
     */
    static SocketAddress getRemoteAddress(int fd);
    
    /**
     * 获取本端地址
     */
    SocketAddress getLocalAddress() const;

    /**
     * 获取本端地址
     */
    static SocketAddress getLocalAddress(int fd);

    /**
     * 连接服务器
     */
    virtual void connect(const SocketAddress& addr);

    /**
     * 发送数据
     * @param data 待发送数据
     * @param dataSize 数据大小
     * @param again 是否无缓冲可写
     * @return 实际发送字节数
     */
    virtual int send(const char* data, int dataSize, bool* again = 0);

    /**
     * 发送数据
     * @param data 待发送数据
     * @param again 是否无缓冲可
     * @return 实际发送字节数
     */
    virtual int send(ByteBuffer& data, bool* again = 0);

    /**
     * 接收数据
     * @param buffer 接收缓冲
     * @param bufferSize 缓冲大小
     * @param again 是否无数据可读
     * @return 实际接收字节数
     */
    virtual int receive(char* buffer, int bufferSize, bool* again = 0);

    /**
     * 接收数据
     * @param buffer 接收数据到缓冲
     * @param again 是否无数据可读
     * @return 实际接收字节数
     */
    virtual int receive(ByteBuffer& buffer, bool* again = 0);

    /**
     * 接收不超过指定字节数的数据到缓冲
     * @param buffer 接收数据到缓冲
     * @param maxBytes 最大接收字节数
     * @param again 是否无数据可读
     * @return 接收字节数
     */
    virtual int receive(ByteBuffer& buffer, int maxBytes, bool* again = 0);

    /**
     * 关闭输入
     */
    virtual void shutdownInput();

    /**
     * 关闭输出
     */
    virtual void shutdownOutput();

    /**
     * 同时关闭输入输出
     */
    virtual void shutdown();
};

///////////////////////////////////////////////////////////////////////ServerSocket

/**
 * 服务端Socket
 */
class ServerSocket : public SocketBase {
public:
    /**
     * 构造函数
     */
    ServerSocket();

    /**
     * 构造函数
     * @param addr 绑定地址
     */
    ServerSocket(const SocketAddress& addr);

    /**
     * 析构函数
     */
    virtual ~ServerSocket();

    /**
     * 获取accept超时
     */
    int getAcceptTimeout() const;

    /**
     * 设置accept超时
     */
    void setAcceptTimeout(int timeout);

    /**
     * 监听端口
     * @param addr 监听端口
     */
    virtual void listen(const SocketAddress& addr);

    /**
     * accept 客户端连接
     */
    virtual Socket* accept();

    /**
     * accept 客户端连接
     */
    virtual int acceptfd(SocketAddress* addr);
};

///////////////////////////////////////////////////////////////////////SocketException

/**
 * socket异常
 */
class SocketException : public SysCallException {
public:
    /**
     * 构造函数
     */
    SocketException(const string& errMsg, bool detail = true)
        : SysCallException(errno, errMsg, detail) {}

    /**
     * 析构函数
     */
    virtual ~SocketException() throw () {}
};

} //namespace mm

#endif

