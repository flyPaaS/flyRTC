#ifndef UCS_PROXY_BYTE_BUFFER_H
#define UCS_PROXY_BYTE_BUFFER_H

#include <vector>
#include <cstring>

namespace Ucs {

/**
 * 字节缓冲
 */
class ByteBuffer {
public:
    /**
     * 构造函数
     */
    ByteBuffer() : _pos(0) {}

    /**
     * 获取大小
     */
    size_t size() const {
        return _storage.size();
    }

    /**
     * 获取是否为空缓冲
     */
    bool empty() const {
        return _storage.empty();
    }

    /**
     * 获取剩余字节数
     */
    size_t remaining() const {
        return _storage.size() - _pos;
    }

    /**
     * 清空
     */
    void clear() {
        _storage.clear();
        _pos = 0;
    }

    /**
     * 加入数据到缓冲
     */
    void write(const void* buffer, size_t size) {
        _storage.insert(_storage.end(), (const char*)buffer, (const char*)buffer + size);
    }

    /**
     * 加入数据到缓冲
     */
    void write(ByteBuffer& buffer) {
        _storage.insert(_storage.end(), buffer.begin(), buffer.begin() + buffer.remaining());
        buffer.clear();
    }

    /**
     * 从缓冲获取数据
     */
    size_t read(void* buffer, size_t size) {
        if (size > remaining()) {
            size = remaining();
        }

        memcpy(buffer, begin(), size);

        setPosition(_pos + size);
        return size;
    }

    /**
     * 从缓冲获取数据, 不移动读位置
     */
    size_t peek(char* buffer, size_t size) {
        if (size > remaining()) {
            size = remaining();
        }

        memcpy(buffer, begin(), size);
        return size;
    }

    /**
     * 跳过数据
     */
    size_t skip(size_t size) {
        if (size > remaining()) {
            size = remaining();
        }

        setPosition(_pos + size);
        return size;
    }

    /**
     * 插入数据
     * @param pos 插入位置
     * @param buffer 待插入数据
     * @param size 待插入数据长度
     */
    void insert(size_t pos, const char* buffer, size_t size) {
        if (pos > this->size()) {
            pos = this->size();
        }

        _storage.insert(_storage.begin() + pos, buffer, buffer + size);
    }

    /**
     * 获取缓冲首地址
     */
    const char* base() const {
        if (_storage.capacity() == 0) {
            return 0;
        }
        return &_storage[0];
    }

    /**
     * 获取缓冲首地址
     */
    char* base() {
        if (_storage.capacity() == 0) {
            return 0;
        }
        return &_storage[0];
    }

    /**
     * 获取缓冲地址
     */
    const char* begin() const {
        if (empty()) {
            return 0;
        }
        return &_storage[0] + _pos;
    }

    /**
     * 获取缓冲地址
     */
    char* begin() {
        if (empty()) {
            return 0;
        }
        return &_storage[0] + _pos;
    }

    /**
     * bool自动转型
     */
    operator bool() const {
        return remaining() > 0;
    }

    /**
     * 获取当前位置
     */
    int getPosition() const {
        return _pos;
    }

    /**
     * 设置当前位置
     */
    void setPosition(size_t pos) {
        if (pos >= size()) {
            clear();
        } else {
            _pos = pos;
        }
    }

    /**
     * 预分配缓冲空间
     */
    void reserve(size_t size) {
        _storage.reserve(size);
    }

    /**
     * 容量
     */
    size_t capacity() const {
        return _storage.capacity();
    }

private:
    std::vector<char> _storage;
    int _pos;
};

} //namespace Ucs

#endif
