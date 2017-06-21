//
//  TCPManager.h
//  UXinClient
//
//  Created by Feng Xing on 13-11-1.
//  Copyright (c) 2013年 UXIN. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AsyncSocket.h"
#import "TCPProtocalDef.h"
#import "UXTCPFrame.h"
@protocol UXTCPManagerDelegate <NSObject>

#pragma mark - 数据收发
- (BOOL)uxOnSend:(UXTCPFrame *)frame;

- (void)uxOnSended:(UXTCPFrame *)frame;

/**
 * 发送失败回调
 * @return YES 已确认，不重复；NO 支持重连后重发
 */
- (int)uxSendFailedWithErrorCode:(NSInteger)errorCode withFrame:(UXTCPFrame *)frame;

- (void)uxOnRecived:(UXTCPFrame *)frame; // parseData

- (void)uxOnDisconnect;
- (void)uxOnConnect;

#pragma mark - 状态改变
- (void)uxOnLoginSuccessed;

- (void)uxOnLoginFailed;

/// 服务器push踢出消息
- (void)uxOnKickout:(NSString *)kickby;

/// 用户主动登陆，或是session过期再次获取失败
- (void)uxOnLogout;

@end

@interface UXTCPManager : NSObject <AsyncSocketDelegate>

@property BOOL backgroundMode;  /// VOIP后台模式
@property int  tcpLoginNetType; /// 登陆的网络类型
@property int  seqnumber;		/// 流水
@property BOOL isOnline;
@property (nonatomic, assign) AsyncSocket *socket;
/**
 *  @brief instance
 */
+ (UXTCPManager *)instance;

- (NSString *)im_ssid;
- (void)setIm_ssid:(NSString *)ssid;

/**
 * @brief 异步向服务器发送请求
 * @prama frame 发送的内容
 * @prama delegate 
 */
- (BOOL)asynSendRequest:(UXTCPFrame *)frame;

/**
 * @brief 注册某个类型的消息回调。
 * @prama nMainCode 主业务码
 * @prama delegate
 */
- (void)registResponsMainWork:(UX_TCP_TYPE)nMainCode delegate:(id)delegate;
/// 取消某个消息的订阅
- (void)unregistResponsMainWork:(UX_TCP_TYPE)nMainCode delegate:(id)delegate;

/**
 * @brief 解析收到的数据
 * @return 包的实际大小
 */
- (int)parseData:(NSData *)data;

- (BOOL)connect2Server;

/// 主动断开连接
- (void)disconnectServer;

- (BOOL)isConnected;
/**
 * @brief 登录
 * 更新im-ssid；获取服务器列表
 */
- (void)httpLogin;
- (void)logout;

/// 判断是否登录。
- (BOOL)hasSsid;

- (void)heartbeat;

- (void)enterBackground;
@end