//
//  CustomUDPListPing.h
//  UXinClient
//
//  Created by Liam on 13-7-16.
//  Copyright (c) 2013年 UXIN. All rights reserved.
//

// Modify by FengXing on 13-12-2
// 自定义voip域名ping
// 输入一组域名，自定义udp ping，然后得到收到由快到慢的一个排序后的ip数组
// 当最快的域名ping收到返回或者超时，发送通知，由外部处理

#import <Foundation/Foundation.h>
#import "GCDAsyncUdpSocket.h"
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>


// 成功ping返回通知
#define kGetTheBestIPSuccessNotification					@"kGetTheBestIPSuccessNotification"

@interface CustomPingPackage : NSObject
@property (copy) NSString *ip;
@property (nonatomic, retain) NSMutableArray *packages;

- (float)loss;      // 丢包率
- (NSInteger)delay; // 延迟ms

- (void)savePingInfo; //保存ping值数据

@end

@interface CustomUDPListPing : NSObject <GCDAsyncUdpSocketDelegate>
{
    NSMutableArray      *_arrayPkgUDPPing;
    GCDAsyncUdpSocket      *_ucsAsyncUdpSocket;
    dispatch_queue_t  _udpQueue;
}

@property (nonatomic,assign) BOOL isFinishPing;//已经完成检测 WLS 20150710
@property (nonatomic,assign) BOOL isPingNow; //是否处于检测状态 WLS 20150710
@property (nonatomic,assign) BOOL didNotPostNoti; //不需要再发送通知设置rtpp
@property (nonatomic,assign) BOOL isIpv6;

// 初始化IP数组
- (void) setPingList:(NSArray *) arrIPList;

// 开始ping
- (void) startPing;
//停止Ping
- (void) stopAllPing;

// 获取IP地址和端口,按ping返回 1、丢包率小；2、延迟低
// 默认排序为传入的顺序
- (NSArray *)bestIPAndPortList;

@end
