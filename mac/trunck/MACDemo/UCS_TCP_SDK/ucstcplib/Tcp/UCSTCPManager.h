//
//  UCSTCPManager.h
//  Tcplib
//
//  Created by FredZhang on 15-4-2.
//  Copyright (c) 2016年 KCTMAC. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "GCDAsyncSocket.h"

#import "KCTTCPDelegateBase.h"
#import "TcpCustomLogger.h"
#import "UCSTCPRealConnecter.h"
#import "NuBSON.h"

#define kSocket_tcp_offLinePush_tag 10

#pragma pack(1)
typedef struct _tagIMTCPFrameHead
{
    UInt32 packageLength;  //数据包长度 4字节
    UInt16 headerLength; //包头  2字节
    UInt16 version;   //版本号协议  2字节
    UInt32 cmd;   //命令号   4字节
    UInt32 seq;   //序列号  4字节
    
} IMTCPFrameHead;  //16字节


typedef struct _socketTagHead{
    UInt32 packageLength;//数据包长度 4字节
    UInt16 headerLength; //包头  2字节
}socketTagHead;

typedef struct _packTagHead{
    UInt16 headerLength; //包头  2字节
    UInt16 packageLength;//数据包长度 2字节
    
}packTagHead;

#pragma pack()

@interface UCSTCPManager : NSObject<GCDAsyncSocketDelegate>

@property (nonatomic, strong)   id  imClient; //ImClient
@property (nonatomic, strong)   id  voipClient; // VoipClient

@property (nonatomic, weak)   id<KCTTCPDelegateBase>   tcpDelegate;

@property int  tcpLoginNetType; /// 登陆的网络类型
@property int  seqnumber;		/// 流水
@property BOOL isClientOnline;  //是否已经登陆标识
@property (nonatomic, strong) GCDAsyncSocket * socket;
@property (nonatomic, strong) NSTimer	*heartbeatTimer;  // 心跳定时器

@property (nonatomic, assign) BOOL offlinePushClosed;   //离线推送是否关闭，yes 关闭 ，no 打开。程序启动时，默认是打开

@property (nonatomic,assign) BOOL beingConnected; //是否处于连接过程中   by WLS 20151026

@property (nonatomic, strong) NSData *loginData;  //登陆验证data
@property (nonatomic, strong) NSData *reLoginData; //重连验证data

@property int    idleSeconds;                /// 空闲时间（用于心跳发送）
@property (nonatomic, assign) BOOL voipSupport;  //是否接入了voip服务
@property (nonatomic, assign) BOOL ImSupport;  //是否接入了Im服务

@property (nonatomic, copy) NSString * ip;  //tcp连接ip
@property (nonatomic, copy) NSString *port; //tcp连接端口
@property (nonatomic,assign) BOOL isLogining; //是否处于登录过程中

+ (instancetype)instance;

-(void)connect2TCP;

-(void)sendData:(NSData *)sendData;


-(void)handleReceiveSocketData:(NSData *)reData;


 - (void)sendLogoutPack;
/**
 *  断开连接
 *  @param isLoginOut yes，主动登出
 */
- (void)disconnectServer:(BOOL) isLoginOut;

/*!
 *  @brief  清空socket，断开连接
 */
- (void)cleanSocketDelegateAndDisconnectSocket;

- (BOOL)isConnected;

- (void)heartbeat;

- (void)resendHeartbeat;

- (void)onConnectSuccessfull;

- (void)setHeartBeatTimer;
- (void)cancelHeartBeatTimer;

- (void)setKeepAlive;



/**
 *  重连
 */
- (void)reConnect;
- (void)cancelReConnect;

- (BOOL)isSocketIPV6;

@end

