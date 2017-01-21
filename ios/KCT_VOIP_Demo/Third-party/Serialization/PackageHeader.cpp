#include "PackageHeader.h"
#include <arpa/inet.h>



const static unsigned short KCT_PROTO_VER_N = htons(ProxyProtocol::PROTO_VER);
const static unsigned short KCT_HEADER_LEN_N = htons(ProxyProtocol::MIN_HEADER_LEN);

PackageHeader::PackageHeader() : packageLength(0), headerLength(0),
        version(1), cmd(0), seq(0) {}

void PackageHeader::reset() {
    packageLength = cmd = seq = 0;
    headerLength = 0;
    version = 1;
}

unsigned int PackageHeader::getPayloadLength() const {
    return packageLength - headerLength;
}

bool PackageHeader::read(KCT::ByteBuffer& buffer) {


    //读取数据
    if (buffer.read(reinterpret_cast<char*>(this), sizeof(PackageHeader))
                != sizeof(PackageHeader)) {
        return false;
    }

    //转为主机字节序
    packageLength = ntohl(packageLength);
    headerLength = ntohs(headerLength);
    version = ntohs(version);
    cmd = ntohl(cmd);
    seq = ntohl(seq);


//     printf(" in PackageHeader cpp , packageLength:%u headerLength:%u version:%u cmd:%u seq:%u \n\n",
//	packageLength , headerLength , version , cmd , seq );

    //校验数据包长度
    if (packageLength < headerLength) {
        return false;
    }

    //校验头部长度
    if (headerLength < ProxyProtocol::MIN_HEADER_LEN || headerLength > ProxyProtocol::MAX_HEADER_LEN) {
        return false;
    }

    //校验版本号
    if (version != ProxyProtocol::PROTO_VER) {
        return false;
    }

    return true;
}

void PackageHeader::write(KCT::ByteBuffer& buffer) {
    PackageHeader temp;
    temp.packageLength = htonl(packageLength);
    temp.headerLength = KCT_HEADER_LEN_N;
    temp.version = KCT_PROTO_VER_N;
    temp.cmd = htonl(cmd);
    temp.seq = htonl(seq);
    buffer.write(reinterpret_cast<const char*>(&temp), sizeof(temp));
}



