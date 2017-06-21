//
//  TCPManager.h
//  UXinClient
//
//  Created by Feng Xing on 13-11-1.
//  Copyright (c) 2013年 UXIN. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "KCTTcpClient.h"
@protocol UCSTCPClientManagerDelegate <NSObject>

#pragma mark - 数据收发




- (void)uxOnDisconnect;
- (void)uxOnConnect;

#pragma mark - 状态改变
- (void)uxOnLoginSuccessed;

- (void)uxOnLoginFailed;


/// 用户主动登陆，或是session过期再次获取失败
- (void)uxOnLogout;

@end

@interface UCSTCPClientManager : NSObject
{
    id                      _backDelegate; //全局回调，UCSServices。
}


@property (nonatomic,assign) KCTTcpClient* tcpClientDelegate; //tcp模块代理;  (WLS 20150630)
@property BOOL isOnline;
@property (nonatomic, assign) BOOL isRelogin; // 判断是登陆还是重连，WLS，2016-05-04
/**
 *  @brief instance
 */
+ (UCSTCPClientManager *)instance;


/**
 * @brief 异步向服务器发送请求
 * @prama frame 发送的内容
 * @prama delegate
 */
- (BOOL)asynSendRequest:(NSData *)sendData;



//tcp链接成功
- (void)ConnectedSuccess;

- (BOOL)isConnected;

-(void)setUCSServiceDelegate:(id)delegate;
@end
