#include "KctUtil.h"
#include <stdio.h>

namespace KCT {

string escape(const string& s) {
    string t;
    for (string::size_type i = 0; i < s.length(); ++i) {
        char c = s[i];
        switch (c) {
        case '\r':
            t.append("\\r");
            break;
        case '\n':
            t.append("\\n");
            break;
        case '\t':
            t.append("\\t");
            break;
        case '\'':
            t.append("\\'");
            break;
        case '\"':
            t.append("\\\"");
            break;
        case '\b':
            t.append("\\b");
            break;
        case '\\':
            t.append("\\\\");
            break;
        default:
            t += c;
            break;
        }
    }
    return t;
}

string escapeJson(const string& s) {
    string t;
    for (string::size_type i = 0; i < s.length(); ++i) {
        char c = s[i];
        switch (c) {
        case '\r':
            t.append("\\r");
            break;
        case '\n':
            t.append("\\n");
            break;
        case '\t':
            t.append("\\t");
            break;
        case '\f':
			t.append("\\f");
			break;
		case '\"':
            t.append("\\\"");
            break;
        case '\b':
            t.append("\\b");
            break;
        case '\\':
            t.append("\\\\");
            break;
        default:
			if (c > 0 && c <= 0x1F) {
				char buf[16];
				sprintf(buf, "\\u%04X", (int)c);
				t += buf; 
			} else {
            	t += c;
			}
            break;
        }
    }
    return t;
}



/////////////////////////////////////////

char Hex::_hexCharTable[16] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
};

unsigned char Hex::toByte(char c) {
    if (c >= '0' && c <= '9') {
        return (unsigned char)(c - '0');
    } else if (c >= 'a' && c <= 'f') {
        return (unsigned char)(10 + c - 'a');
    } else if (c >= 'A' && c <= 'F') {
        return (unsigned char)(10 + c - 'A');
    } else { //出现异常字符
        return 0xff;
    }
}

int Hex::toBytes(const char* s, unsigned char* bytes) {
    unsigned char* p = bytes;
    unsigned char high, low;
    while (*s) {
        //高4位
        high = toByte(*s++);
        if (high == 0xff || *s == '\0') {
            return -1;
        }

        //低4位
        low = toByte(*s++);
        if (low == 0xff) {
            return -1;
        }

        *p++ = (unsigned char)((high << 4) | low);
    }
    return p - bytes;
}

void Hex::toString(const unsigned char* bytes, int n, char* s) {
    for (int i = 0; i < n + n;) {
        s[i++] = _hexCharTable[*bytes >> 4];
        s[i++] = _hexCharTable[(*bytes++) & 0xf];
    }
    s[n + n] = '\0';
}

///////////////////////////////////////////////////////////MessageQueue
/*

MessageQueue::MessageQueue() : _mqd(-1) {}

MessageQueue::~MessageQueue() {
    close();
}

void MessageQueue::open(const string& path, int oflag) {
    _mqd = mq_open(path.c_str(), oflag);
    if (_mqd == (mqd_t)-1) {
        throw MQException(errno, "mq_open error");
    }
}

bool MessageQueue::send(const char* msg, size_t msgLen, unsigned int prio) {
    if (mq_send(_mqd, msg, msgLen, prio) == 0) {
        return true;
    }

    if (errno != EAGAIN && errno != EINTR) {
        throw MQException(errno, "mq_send error");
    }

    return false;
}

bool MessageQueue::receive(char* msg, size_t msgLen, unsigned int* prio) {
    if (mq_receive(_mqd, msg, msgLen, prio) == 0) {
        return true;
    }

    if (errno != EAGAIN && errno != EINTR) {
        throw MQException(errno, "mq_receive error");
    }

    return false;
}

void MessageQueue::close() {
    if (_mqd != (mqd_t)-1) {
        mq_close(_mqd);
        _mqd = -1;
    }
}
*/
}


