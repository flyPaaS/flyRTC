//
//  CustomUDPListPing.m
//  UXinClient
//
//  Created by Liam on 13-7-16.
//  Copyright (c) 2013年 UXIN. All rights reserved.
//

#import "CustomUDPListPing.h"

#define UDP_SEND_COUNT 10  //udp发包为10个包 add by WLS 20150819



@implementation CustomPingPackage

- (NSMutableArray*)packages
{
    if (_packages == nil) {
        _packages = [[NSMutableArray alloc] initWithCapacity:UDP_SEND_COUNT];
    }
    return _packages;
}

- (float)loss
{
    int recv = 0;
    for (NSValue *v in self.packages) {
        if (v) {
            NSRange range = [v rangeValue];
            if (range.length != INT32_MAX) {
                recv++;
            }
        }
    }
    
    
    return ((float)(UDP_SEND_COUNT-recv))/UDP_SEND_COUNT;
    
}

- (NSInteger)delay
{
    int recv = 0;
    NSInteger elapse = 0;
    
    for (NSValue *v in self.packages) {
        if (v) {
            NSRange range = [v rangeValue];
            if (range.length != INT32_MAX) {
                recv++;
                elapse += range.length;
                
            }
        }
    }
    if (recv == 0) {
        //如果没有收到包 延时定为3000ms
        return 3000;
    }
    
    //计算规则  add by WLS 20150819
    /*
     
     */
    
    NSInteger delay = elapse/recv;
    
    return delay;
}

//保存ping值数据
- (void)savePingInfo{
    
    
    NSString * delayStr = @"";
    NSString * ip = [NSString stringWithFormat:@"当前 ip : %@",self.ip];
    
    int recv = 0;
    NSInteger elapse = 0;
    
    for (NSValue *v in self.packages) {
        if (v) {
            NSRange range = [v rangeValue];
            if (range.length != INT32_MAX) {
                recv++;
                
                elapse += range.length;
                
            }
            
            delayStr = [delayStr stringByAppendingString:[NSString stringWithFormat:@"\n%@",[NSString stringWithFormat:@"收包延时：%f ms",range.length !=INT32_MAX?range.length/1000.0*1000:0.00]]];
            
            
        }
    }
    if (recv == 0) {
        [[SDKLogManager instance]saveSDKLogInfo:@"UDP ping值数据存储" withDetail:[NSString stringWithFormat:@"%@\n%@\n%@",ip,delayStr,[NSString stringWithFormat:@"收包延时3000ms，丢包率100"]]];
        return;
    }
    
    //    elapse = elapse  * 1000 / CLOCKS_PER_SEC;
    
    
    //[[SDKLogManager instance]saveSDKLogInfo:@"UDP ping值数据存储" withDetail:[NSString stringWithFormat:@"%@\n%@\n%@",ip,delayStr,[NSString stringWithFormat:@"平均延时: %f ms   平均丢包率：%d",elapse/(10.0*recv),(int)(((float)(UDP_SEND_COUNT-recv))/UDP_SEND_COUNT*100)]]];
    [[SDKLogManager instance]saveSDKLogInfo:@"UDP ping值数据存储" withDetail:[NSString stringWithFormat:@"%@\n%@\n%@",ip,delayStr,[NSString stringWithFormat:@"平均延时: %f ms   平均丢包率：%d",elapse/recv,(int)(((float)(UDP_SEND_COUNT-recv))/UDP_SEND_COUNT*100)]]];
    
    
    //测试 注销掉
    //    [[NSNotificationCenter defaultCenter] postNotificationName:@"SENDUDP" object:[NSDictionary dictionaryWithObjectsAndKeys:[NSString stringWithFormat:@"%@\n%@\n%@",ip,delayStr,[NSString stringWithFormat:@"平均延时: %f ms   平均丢包率：%d",TTm/10,(int)(((float)(UDP_SEND_COUNT-recv))/UDP_SEND_COUNT*100)]],@"info", nil]];
    
}



@end

@implementation CustomUDPListPing

@synthesize isFinishPing = _isFinishPing;
@synthesize isPingNow = _isPingNow;

- (instancetype)init
{
    self = [super init];
    _udpQueue = dispatch_queue_create("udpSend", NULL);
    
    return self;
}


// 初始化IP数组，ping超时时间
- (void) setPingList:(NSArray *) arrIPList
{
    
    //设置的时候 先停止udp动作 WLS
    [self stopAllPing];
    
    if (_arrayPkgUDPPing) {
        //移除所有的ip WLS
        [_arrayPkgUDPPing removeAllObjects];
    }else{
        _arrayPkgUDPPing = [[NSMutableArray alloc] init];
        
    }
    
    
    for (id ip in arrIPList) {
        CustomPingPackage *pp = [[CustomPingPackage alloc] init];
        pp.ip = ip;
        [_arrayPkgUDPPing addObject:pp];
        [pp release];
    }
}

- (void) dealloc
{
    [_arrayPkgUDPPing release];
    [_ucsAsyncUdpSocket release];
    dispatch_release(_udpQueue);
    [super dealloc];
}

NSTimeInterval _getTickCount() {
    
    struct timeval tv;
    
    if (gettimeofday(&tv, NULL) != 0)
        return 0;
    
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}


static struct sockaddr_in6 * getIpv6Adr(const char *ipv4Adr,int port)
{
    if (ipv4Adr == NULL) {
        return NULL;
    }
    NSString *portStr = [NSString stringWithFormat:@"%d", port];
    struct addrinfo hints, *res, *res0;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = PF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    
    int gai_error = getaddrinfo(ipv4Adr, [portStr UTF8String], &hints, &res0);
    if (gai_error)
    {
        perror("");
        freeaddrinfo(res0);
    }
    else
    {
        const char* pszTemp;
        for(res = res0; res; res = res->ai_next)
        {
            char buf[32];
            if (res->ai_family == AF_INET)
            {
                // Found IPv4 address
                printf("------ipv4-----");
            }
            else if (res->ai_family == AF_INET6)
            {
                struct sockaddr_in6 *sockaddr = (struct sockaddr_in6 *)res->ai_addr;
                in_port_t *portPtr = &sockaddr->sin6_port;
                if ((portPtr != NULL) && (*portPtr == 0)) {
                    *portPtr = htons(port);
                }
                pszTemp = inet_ntop(AF_INET6, &sockaddr->sin6_addr, buf, sizeof(buf));
                //printf("\n----gg---ipv4:%s-%d---ipv6:%s----%d--\n",ipv4Adr,strlen(ipv4Adr),pszTemp,strlen(pszTemp));
                // Found IPv6 address
                // Wrap the native address structure and add to list
                return sockaddr;
            }
        }
        freeaddrinfo(res0);
    }
    return NULL;
}

- (void)startPingWithIpv6
{
    dispatch_async(_udpQueue, ^(void){
        
        int clientSocketId = -1;
        clientSocketId = socket(AF_INET6, SOCK_DGRAM, 0);
        if (clientSocketId < 0) {
            NSLog(@"create socket fail");
        }
        //set socket timeout
        struct timeval timeout = {2,0};
        int ret = setsockopt(clientSocketId, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
        if (ret == 0) {
            NSLog(@"设置socket send 超时成功");
        }
        ret = setsockopt(clientSocketId, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
        if (ret == 0) {
            NSLog(@"设置socket rec 超时成功");
        }
        
        for (int i = 0; i < [_arrayPkgUDPPing count]; ++i) {
            CustomPingPackage *pkg = [_arrayPkgUDPPing objectAtIndex:i];
            NSString *IPPort = pkg.ip;
            NSString *strIP = @"";
            NSString *strPort = @"518"; //20140913 RTPP 若不设置端口默认为7801
            
            NSRange range = [IPPort rangeOfString:@":"];//判断是否包含“:”
            if (range.location != NSNotFound) { //如果不是没有找到端口，则将ip和端口分开。
                strIP = [IPPort substringToIndex:range.location];
                strPort = [IPPort substringFromIndex:range.location + 1];
            } else {//如果没有端口则使用默认的518端口
                strIP = IPPort;
            }
            
            socklen_t addrlen;
            struct sockaddr_in6 *client_sockaddr = NULL;
            addrlen = sizeof(struct sockaddr_in6);
            client_sockaddr = getIpv6Adr([strIP UTF8String], [strPort intValue]);
            
            char buffer[10] = {0};
            ssize_t len;
            
            for (int k = 0; k < UDP_SEND_COUNT; k++) {
                int tag = i*UDP_SEND_COUNT+k;
                
                bzero(buffer, sizeof(buffer));
                sprintf(buffer, "pong %d",tag);
                len = sendto(clientSocketId, buffer, sizeof(buffer), 0, (struct sockaddr *)client_sockaddr, addrlen);
                NSTimeInterval sendUdpTime = _getTickCount();
                
                
                pkg.packages[k] = [NSValue valueWithRange:NSMakeRange(sendUdpTime, INT32_MAX)];
                
                if (len > 0) {
                    NSLog(@"发送成功");
                    //printf("--%d--send time:%f\n",tag,sendUdpTime);
                } else {
                    NSLog(@"发送失败");
                    perror("--send fail---");
                }
                
                bzero(buffer, sizeof(buffer));
                len = recvfrom(clientSocketId, buffer, sizeof(buffer), 0, (struct sockaddr *)client_sockaddr, &addrlen);
                if (len > 0) {
                    NSLog(@"接收成功");
                    NSTimeInterval recvUdpTime = _getTickCount();
                    //printf("--%d--recv time:%f\n",tag,recvUdpTime);
                    
                    CustomPingPackage *pkg2 = _arrayPkgUDPPing[tag/UDP_SEND_COUNT];
                    if (pkg2) {
                        //        [[SDKLogManager instance]saveSDKLogInfo:@"ping机制数组判断" withDetail:[NSString stringWithFormat:@"发送的第%ld个包---数组里有%ld包",(tag%UDP_SEND_COUNT),pkg2.packages.count] ];
                        
                        NSValue *v = pkg2.packages[tag%UDP_SEND_COUNT];
                        if (v) {
                            
                            //收包的时间
                            //NSTimeInterval receiveUdpTime = [[NSDate date] timeIntervalSince1970]*10000; //收包的时间获取的是秒单位 转化成毫秒 保留一位小数. add by WLS 20150819
                            NSRange range = [v rangeValue];
                            range.length = recvUdpTime - range.location; //收包的时间减去发包的时间 获取到秒的10000倍数值 转换成毫秒需要/10000.0*1000 这样可以保留毫秒单位的一位小数;
                            pkg2.packages[tag%UDP_SEND_COUNT] = [NSValue valueWithRange:range];
                        }
                    }
                } else {
                    NSLog(@"接收失败，或者接收超时");
                    perror("接收失败");
                }
            }
        }
        close(clientSocketId);
    });
}
- (void)startPingWithIpv4
{
    dispatch_async(_udpQueue, ^(void){
        
        int clientSocketId = -1;
        clientSocketId = socket(AF_INET, SOCK_DGRAM, 0);
        if (clientSocketId < 0) {
            NSLog(@"create socket fail");
        }
        //set socket timeout
        struct timeval timeout = {2,0};
        int ret = setsockopt(clientSocketId, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));
        if (ret == 0) {
            NSLog(@"设置socket send 超时成功");
        }
        ret = setsockopt(clientSocketId, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
        if (ret == 0) {
            NSLog(@"设置socket rec 超时成功");
        }
        
        for (int i = 0; i < [_arrayPkgUDPPing count]; ++i) {
            CustomPingPackage *pkg = [_arrayPkgUDPPing objectAtIndex:i];
            NSString *IPPort = pkg.ip;
            NSString *strIP = @"";
            NSString *strPort = @"518"; //20140913 RTPP 若不设置端口默认为7801
            
            NSRange range = [IPPort rangeOfString:@":"];//判断是否包含“:”
            if (range.location != NSNotFound) { //如果不是没有找到端口，则将ip和端口分开。
                strIP = [IPPort substringToIndex:range.location];
                strPort = [IPPort substringFromIndex:range.location + 1];
            } else {//如果没有端口则使用默认的518端口
                strIP = IPPort;
            }
            
            socklen_t addrlen;
            struct sockaddr_in client_sockaddr;
            addrlen = sizeof(struct sockaddr_in);
            bzero(&client_sockaddr, addrlen);
            client_sockaddr.sin_family = AF_INET;
            client_sockaddr.sin_addr.s_addr = inet_addr([strIP UTF8String]);
            client_sockaddr.sin_port = htons([strPort intValue]);
            
            char buffer[10] = {0};
            ssize_t len;
            
            for (int k = 0; k < UDP_SEND_COUNT; k++) {
                int tag = i*UDP_SEND_COUNT+k;
                
                bzero(buffer, sizeof(buffer));
                sprintf(buffer, "pong %d",tag);
                len = sendto(clientSocketId, buffer, strlen(buffer), 0, (struct sockaddr *)&client_sockaddr, addrlen);
                NSTimeInterval sendUdpTime = _getTickCount();
                
                //printf("--%d--send time:%f\n",tag,sendUdpTime);
                pkg.packages[k] = [NSValue valueWithRange:NSMakeRange(sendUdpTime, INT32_MAX)];
                
                if (len > 0) {
                    NSLog(@"发送成功");
                } else {
                    NSLog(@"发送失败");
                }
                
                bzero(buffer, sizeof(buffer));
                len = recvfrom(clientSocketId, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_sockaddr, &addrlen);
                if (len > 0) {
                    NSLog(@"接收成功");
                    NSTimeInterval recvUdpTime = _getTickCount();
                    //printf("--%d--recv time:%f ip:%s\n",tag,recvUdpTime,[strIP UTF8String]);
                    
                    CustomPingPackage *pkg2 = _arrayPkgUDPPing[tag/UDP_SEND_COUNT];
                    if (pkg2) {
                        //        [[SDKLogManager instance]saveSDKLogInfo:@"ping机制数组判断" withDetail:[NSString stringWithFormat:@"发送的第%ld个包---数组里有%ld包",(tag%UDP_SEND_COUNT),pkg2.packages.count] ];
                        
                        NSValue *v = pkg2.packages[tag%UDP_SEND_COUNT];
                        if (v) {
                            
                            //收包的时间
                            //NSTimeInterval receiveUdpTime = [[NSDate date] timeIntervalSince1970]*10000; //收包的时间获取的是秒单位 转化成毫秒 保留一位小数. add by WLS 20150819
                            NSRange range = [v rangeValue];
                            range.length = recvUdpTime - range.location; //收包的时间减去发包的时间 获取到秒的10000倍数值 转换成毫秒需要/10000.0*1000 这样可以保留毫秒单位的一位小数;
                            pkg2.packages[tag%UDP_SEND_COUNT] = [NSValue valueWithRange:range];
                        }
                    }
                } else {
                    NSLog(@"接收失败，或者接收超时");
                }
            }
        }
        close(clientSocketId);
    });
}

// 开始ping
- (void) startPing
{
    self.isPingNow = YES;
    self.isFinishPing = NO;
    if (self.isIpv6) {
        [self startPingWithIpv6];
    } else {
        [self startPingWithIpv4];
    }
    
    // 启动超时
    [self performSelector:@selector(pingTimeout) withObject:nil afterDelay:3.0f];
}

// 开始ping
- (void) startPing2
{
    
    self.isPingNow = YES;
    self.isFinishPing = NO;
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(void){
        
        
        if (_ucsAsyncUdpSocket == nil) {
            _ucsAsyncUdpSocket = [[GCDAsyncUdpSocket alloc]initWithDelegate:self delegateQueue:dispatch_get_main_queue()];
        }
        else
        {
            [_ucsAsyncUdpSocket release];
            _ucsAsyncUdpSocket = nil;
            _ucsAsyncUdpSocket = [[GCDAsyncUdpSocket alloc]initWithDelegate:self delegateQueue:dispatch_get_main_queue()];
        }
        
        NSError * error = nil;
        [_ucsAsyncUdpSocket bindToPort:51888 error:&error];
        if (error) {//监听错误打印错误信息
            NSLog(@"error:%@",error);
        }else {//监听成功则开始接收信息
            [_ucsAsyncUdpSocket beginReceiving:&error];
        }
        
        
        for (int i = 0; i < [_arrayPkgUDPPing count]; ++i) {
            CustomPingPackage *pkg = [_arrayPkgUDPPing objectAtIndex:i];
            NSString *IPPort = pkg.ip;
            NSString *strIP = @"";
            NSString *strPort = @"518"; //20140913 RTPP 若不设置端口默认为7801
            
            NSRange range = [IPPort rangeOfString:@":"];//判断是否包含“:”
            if (range.location != NSNotFound) { //如果不是没有找到端口，则将ip和端口分开。
                strIP = [IPPort substringToIndex:range.location];
                strPort = [IPPort substringFromIndex:range.location + 1];
            } else {//如果没有端口则使用默认的518端口
                strIP = IPPort;
            }
            
            
            //发包的时间
            //NSTimeInterval sendUdpTime = [[NSDate date] timeIntervalSince1970]*10000; //发包的时间获取的是秒单位 转化成毫秒 保留一位小数. add by WLS 20150819
            
            for (int k = 0; k < UDP_SEND_COUNT; k++) {
                int tag = i*UDP_SEND_COUNT+k;
                NSString *sendStr = [NSString stringWithFormat:@"pong %d",tag];
                NSData *sendData = [sendStr dataUsingEncoding:NSUTF8StringEncoding];
                [_ucsAsyncUdpSocket sendData:sendData toHost:strIP port:[strPort intValue] withTimeout:0.1f tag:tag];
                NSTimeInterval sendUdpTime = _getTickCount();
                printf("--%d--send time:%f\n",tag,sendUdpTime);
                //                NSLog(@"Begin pong:%@:%@, with tag:%d", strIP, strPort, tag);
                pkg.packages[k] = [NSValue valueWithRange:NSMakeRange(sendUdpTime, INT32_MAX)];
                
            }
        }
    
    });
    // 启动超时
    [self performSelector:@selector(pingTimeout) withObject:nil afterDelay:3.0f];
}



// 停止所以ping操作
- (void) stopAllPing
{
    _ucsAsyncUdpSocket.delegate = nil;
    //不处于检测状态中 WLS 20150710
    self.isPingNow = NO;
    //没有完成ping检测
    self.isFinishPing = NO;
    [_ucsAsyncUdpSocket close];
    
}

// ping超时
- (void) pingTimeout
{
    [self stopAllPing];
    //完成ping值检测 WLS 20150710
    self.isFinishPing = YES;
    //不处于检测状态中 WLS 20150710
    self.isPingNow = NO;
    if (self.didNotPostNoti) {
        //不需要发送通知 add by WLS 20151118
        return;
    }
    [[NSNotificationCenter defaultCenter] postNotificationName:kGetTheBestIPSuccessNotification object:nil];
}

// 收到回应
- (void) onReceiveWithTag:(long) tag
{
    CustomPingPackage *pkg = _arrayPkgUDPPing[tag/UDP_SEND_COUNT];
    if (pkg) {
//        [[SDKLogManager instance]saveSDKLogInfo:@"ping机制数组判断" withDetail:[NSString stringWithFormat:@"发送的第%ld个包---数组里有%ld包",(tag%UDP_SEND_COUNT),pkg.packages.count] ];
        
        NSValue *v = pkg.packages[tag%UDP_SEND_COUNT];
        if (v) {
            
            //收包的时间
            //NSTimeInterval receiveUdpTime = [[NSDate date] timeIntervalSince1970]*10000; //收包的时间获取的是秒单位 转化成毫秒 保留一位小数. add by WLS 20150819
            NSTimeInterval receiveUdpTime = _getTickCount();
            printf("--%d---rece time:%f\n",tag,receiveUdpTime);
            NSRange range = [v rangeValue];
            range.length = receiveUdpTime - range.location; //收包的时间减去发包的时间 获取到秒的10000倍数值 转换成毫秒需要/10000.0*1000 这样可以保留毫秒单位的一位小数;
            pkg.packages[tag%UDP_SEND_COUNT] = [NSValue valueWithRange:range];
        }
    }
}

// 获取IP地址和端口,按ping返回速度由快到慢排序
// 默认排序为传入的顺序
- (NSArray *)bestIPAndPortList
{
    NSMutableArray *sortedUDPArray = [NSMutableArray arrayWithArray:_arrayPkgUDPPing];
    [sortedUDPArray sortWithOptions:NSSortStable usingComparator:^NSComparisonResult(id obj1, id obj2) {
        return [obj1 loss] > [obj2 loss];
    }];
    [sortedUDPArray sortWithOptions:NSSortStable usingComparator:^NSComparisonResult(id obj1, id obj2) {
        return [obj1 delay] > [obj2 delay];
    }];
    
    return sortedUDPArray;
}

#pragma mark -
#pragma UCSAsyncUdpSocketDelegate

- (void)udpSocket:(GCDAsyncUdpSocket *)sock didReceiveData:(NSData *)data fromAddress:(NSData *)address withFilterContext:(id)filterContext
{
    NSString *result = [[NSString alloc] initWithData:data  encoding:NSUTF8StringEncoding];
    NSArray *resArray = [result componentsSeparatedByString:@" "];
    NSString *tag = [resArray objectAtIndex:1];
    int iTag = [tag intValue];
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^(void) {
        [self onReceiveWithTag:iTag];
    });
    //[self onReceiveWithTag:iTag];
}

- (void)udpSocket:(GCDAsyncUdpSocket *)sock didSendDataWithTag:(long)tag
{
    NSLog(@"发送信息成功");
}

- (void)udpSocket:(GCDAsyncUdpSocket *)sock didNotSendDataWithTag:(long)tag dueToError:(NSError *)error
{
    NSLog(@"发送信息失败");
}


@end
