//
//  KCTTcpClient.h
//  Tcplib
//
//  Created by wenqinglin on 15-4-11.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "KCTTCPDelegateBase.h"


//add by wenqinglin
extern NSString * const UCSNotiEngineSucess;

static NSString * const TCPConnectStateNotification = @"TCPConnectStateNotification";
static NSString * const KCTCPDisConnectNotification = @"KCTCPDisConnectNotification" ;  // tcp断开连接
static NSString * const KCTCPNoNetWorkNotification = @"KCTCPNoNetWorkNotification"   ;  // 没网




@class KCTTCPTransParentRequest;




/*!
 *  @brief KCTTcpClient是KCT  Tcp Sdk核心能力类，为IM、Voip等能力提供支持。
 */
@interface KCTTcpClient : NSObject

/**
 *  sdk版本号
 */
@property(nonatomic, copy, readonly)NSString * version;


/*!
 *  @brief  实例化KCTTcpClient的一个单例对象
 
 *  @discussion 这个方法是生成KCTTcpClient的唯一方法，请不要使用init()等其他初始化方法，否知会出现未知的错误
 
 *  @return KCTTcpClient类型的单例对象
 */
+ (instancetype)sharedTcpClientManager;


/*!
 *  @brief  设置代理.
 *
 *  @discussion  如果需要通过回调监控tcp连接状态之类的,就必须设置这个代理
 *
 *  @param delegate tcp事件回调代理
 */
- (void)setTcpDelegate:(id <KCTTCPDelegateBase>) delegate;


/**
 *  设置离线推送的环境和设备号deviceToke。如果需要使用推送离线推送，则必须调用此方法。
 *
 *  @param environment 离线推送的环境.具体见KCTPushEnvironment，不设置的话,默认是开发环境。
 *  @param deviceToken 从系统获取到的设备号deviceToken。
 */
- (void)setPushEnvironment:(KCTPushEnvironment)environment
               deviceToken:(NSData *)deviceToken;



//add by wenqinglin
//明文登录方式：
- (void)connect:(NSString *)accountSid accountToken:(NSString *)accountToken clientNumber:(NSString *)clientNumber clientPwd:(NSString *)clientPwd success:(void (^)(NSString *userId))success
        failure:(void (^)(KCTError *error))failure;


//密文登录方式：
- (void)connect:(NSString *)token success:(void (^)(NSString *userId))success
        failure:(void (^)(KCTError *error))failure;

/*!
 *  @brief  连接云平台.
 
 *  @discussion (注意: 除非是为了兼容旧的IMSDK,否则请使用这个登陆方式)。
 
 *  @param token        加密的token
 *  @param success      成功回调
 *  @param failure      登陆失败回调，注意：登陆账号被踢线（在其他地方登录）也会回调这个block
 */
- (void)login_connect;


/*!
 *  @brief  查询客户端与云平台的连接状态,
 
 *  @discussion
 *
 *  @return yes，已连接；no，断开连接
 */
- (BOOL)login_isConnected;

/*!
 *  @brief  查询当前网络状态
 
 *  @discussion
 *
 *  @return 当前网络状态
 */
- (KCTNetworkStatus)getCurrentNetWorkStatus;


/*!
 *  @brief  断开客户端和云平台的连接,并且根据flag标识的bool值选择是否关闭离线推送。
 *
 *  @param flag 是否关闭推送的bool值。YES，关闭。NO，不关闭。
 *
 *  @discussion 断开连接。
 */
- (void)login_uninitWithFlag:(BOOL)flag;

- (BOOL)isSocketIPV6;

#pragma mark 透传

/**
 *  @brief 发送透传数据
 *
 *  @param request    透传请求类
 *  @param success 发送成功时回调
 *  @param failure 发送失败时回调
 */
- (KCTTCPTransParentRequest *)sendTransParentData:(KCTTCPTransParentRequest *)request
                                          success:(void (^)(KCTTCPTransParentRequest *request))success
                                          failure:(void (^)(KCTTCPTransParentRequest *request, KCTError *error))failure;






@end
