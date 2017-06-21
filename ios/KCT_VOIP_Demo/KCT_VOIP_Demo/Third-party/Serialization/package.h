
#ifndef PROXY_PACKAGE_H
#define PROXY_PACKAGE_H

#include <stdlib.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "iMd5.h"

#include "kctpkg.h"
#include "kctcompress.h"
#include "kctsyncdef.h"
#include "xyssl.h"
#include "PackageHeader.h"
#include "IMmsgstruct.h"
#include "ProxyProtocol.h"
#include <map>
#include <string>
#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
using namespace std;
using namespace KCT;
using namespace Comm;


typedef unsigned char uint8_t ;


#define IN
#define OUT
int  pack(unsigned int cmd, BodyHead_t* bodyHead, void* bodyData, char* buf, unsigned int& size); 

int  unpack(unsigned int cmd, BodyHead_t* bodyHead, void* bodyData, char* buf, unsigned int size);

#endif
