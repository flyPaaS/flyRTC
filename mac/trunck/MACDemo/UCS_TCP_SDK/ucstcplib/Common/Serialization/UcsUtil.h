/*****************************************************************************
 *
 * File:        Util.h
 * Author:      iotazhang
 * Revision:    $Id$
 * Description: 工具类
 *
 *****************************************************************************/

#ifndef UCS_PROXY_UTIL_H
#define UCS_PROXY_UTIL_H

#if 0
#include <mqueue.h>
#endif

#include <sstream>
#include <exception>
#include <cerrno>
#include <cstring>


#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>

namespace Ucs {

using std::string;
using std::ostringstream;
using std::exception;

/**
 * 转字符串
 */
template <class T>
string str(const T& t) {
    ostringstream os;
    os << t;
    return os.str();
}

/**
 * 转义字符串
 */
string escape(const string& s);

/**
 * json转义
 */
string escapeJson(const string& s);

/**
 * 线程适配器
 */
template <class T>
class ThreadAdapter {
public:
    /**
     * 构造函数
     */
    ThreadAdapter(T* t) : _t(t) {}

    /**
     * 运行
     */
    void operator()() {
        _t->run();
    }

private:
    T* _t;
};

/**
 * 十六进制字符处理
 */
class Hex {
public:
    /**
     * 将字符转为对应字节, 如'1'转为1
     * @param c 字符
     * @return 字节, 异常字符返回 0xff
     */
    static unsigned char toByte(char c);

    /**
     * 转为字节数组
     * @param s 源字符串
     * @param bytes 结果缓存, 大小至少s的一半大小
     * @return 字节数, 出错时返回-1
     */
    static int toBytes(const char* s, unsigned char* bytes);

    /**
     * 转为字符串
     */
    static void toString(const unsigned char* bytes, int n, char* s);

private:
    static char _hexCharTable[16];
};

/**
 * 禁止复制
 */
class Noncopyable {
protected:
    Noncopyable() {}
    ~Noncopyable() {}
private:
    Noncopyable(const Noncopyable&);
    const Noncopyable& operator=(const Noncopyable&);
};

/**
 * Posix 消息队列
 */
class MessageQueue : public Noncopyable {
public:
    /**
     * 构造函数
     */
    MessageQueue();

    /**
     * 析构函数
     */
    virtual ~MessageQueue();

    /**
     * 打开消息队列
     * @param path 消息队列路径
     * @param oflag 打开参数
     */
    void open(const string& path, int oflag = O_CREAT | O_RDWR);

    /**
     * 发送消息
     */
    bool send(const char* msg, size_t msgLen, unsigned int prio = 100);

    /**
     * 接收消息
     */
    bool receive(char* msg, size_t msgLen, unsigned int* prio = 0);

    /**
     * 关闭消息队列
     */
    void close();

private:

#if 0
    mqd_t   _mqd;
#endif
	
};

/**
 * 系统调用异常
 */
class SysCallException : public exception {
public:
    /**
     * 构造函数
     */
    SysCallException(int errCode, const string& errMsg, bool detail = true) : _errCode(errCode), _errMsg(errMsg) {
        if (detail) {
            _errMsg.append(", ").append(::strerror(errno));
        }
    }

    /**
     * 析构函数
     */
    virtual ~SysCallException() throw () {}

    /**
     * 获取错误代码
     */
    int getErrorCode() const throw () {
        return _errCode;
    }

    /**
     * 获取错误信息
     */
    const char* what() const throw () {
        return _errMsg.c_str();
    }

protected:
    int     _errCode;
    string  _errMsg;
};

/**
 * 消息队列异常
 */
class MQException : public SysCallException {
public:
    /**
     * 构造函数
     */
    MQException(int errCode, const string& errMsg, bool detail = true)
        : SysCallException(errCode, errMsg, detail) {}

    /**
     * 析构函数
     */
    virtual ~MQException() throw () {}
};

} //namespace Ucs

#endif
