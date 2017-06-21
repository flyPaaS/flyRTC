//
//  UCSTCPManager.m
//  UXinClient
//
//  Created by Feng Xing on 13-11-1.
//  Copyright (c) 2013年 UXIN. All rights reserved.
//
#import "UCSTCPClientManager.h"
#import "SoftphoneManager.h"
#import <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#import "UCSHttpRequest.h"
#import "PublicFunc.h"
#import "UCSStatiRequestManager.h"
#import "UCSDialViewController.h"
#import "CallLogManager.h"

#import "UCSCPSManager.h" //拉取cps
#import "UCSStatiRequestManager.h" //上报数据
#import "UCSNetStaticInterface.h" //拉取 rtpp
#import "KCTMessageFrame.h"
#import "MacVideoView.h"



@interface UCSTCPClientManager ()


@end

static UCSTCPClientManager *g_instance = nil;

#define NS_INT(x) [NSNumber numberWithInt:(x)]
#define UCSContinueLive 0   // WLS，2016-04-19，通话续活时间，当tcp断开后，延迟10s再通知组件断开连接，10s内重连成功，则只需要更新状态

@implementation UCSTCPClientManager
{
    BOOL _isOnline;                         // 是否在线，用于网络状态变化重连
    
    NSTimer *_continueLiveTimer;        // WLS，2016-04-19，续活定时器，当tcp断开10s内重新连接上，则不通知组件断开
}
@synthesize tcpClientDelegate = _tcpClientDelegate;

-(void)setUCSServiceDelegate:(id)delegate
{
    _backDelegate =[delegate retain];
    
}

+ (UCSTCPClientManager *) instance
{
    if (nil == g_instance) {
        g_instance = [[UCSTCPClientManager alloc] init];
    }
    
    return g_instance;
}

- (id)init
{
    if (self = [super init]) {
        
        
        /**
         @author WLS, 15-12-08 18:12:47
         
         监听系统前后台情况
         
         */
//        [[NSNotificationCenter defaultCenter] addObserver:self
//                                                 selector:@selector(enterForeground)
//                                                     name:UIApplicationWillEnterForegroundNotification
//                                                   object:nil];
        
//        [[NSNotificationCenter defaultCenter] addObserver:self
//                                                 selector:@selector(enterBackground)
//                                                     name:UIApplicationDidEnterBackgroundNotification
//                                                   object:nil];
        
    }
    return self;
}

- (void)dealloc
{
    _tcpClientDelegate = nil;
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [_backDelegate release];
    
    [self stopContinueLiveTimer];
    
    [super dealloc];
}




- (BOOL)isConnected
{
    return [_tcpClientDelegate login_isConnected];
}




- (BOOL)asynSendRequest:(NSData *)sendData
{
    
    if (_tcpClientDelegate) {
        [self send:sendData];
        return YES;
    }else{
        
        [[SDKLogManager instance]saveSDKLogInfo:@"发送包" withDetail:@"tcp未连接,发送包失败"];
        
    }
    
    return NO;
}

- (void)send:(NSData *)sendData
{
    
    if (_tcpClientDelegate) {
        
        
        if ([_tcpClientDelegate respondsToSelector:@selector(sendData:modelType:)]) {
            [_tcpClientDelegate performSelector:@selector(sendData:modelType:) withObject:sendData withObject:[NSNumber numberWithInt:2]];
        }
        
        
    }
    
    
}




#pragma mark - UCSTCPManagerDelegate





- (void)ConnectedSuccess
{
    _isOnline = YES;
    
    
    /**
     @author WLS, 16-04-19 10:04:20
     
     重连成功后，将续活的定时器停止
     */
    [self stopContinueLiveTimer];
    
    
    SoftphoneManager * delegate =  [SoftphoneManager instance];
    
    
    if ([delegate respondsToSelector:@selector(uxOnConnect)]) {
        [delegate performSelector:@selector(uxOnConnect)];
    }

    
}


/**
 @author WLS, 16-05-04 12:05:05
 
 停止续活通话的定时器
 */
- (void)stopContinueLiveTimer{
    if (_continueLiveTimer) {
        if ([_continueLiveTimer isValid]) {
            [_continueLiveTimer invalidate];
        }
        [_continueLiveTimer release];
        _continueLiveTimer = nil;
    }
}

/**
 @author WLS, 16-04-19 10:04:50
 
 如果10s内tcp连接上了，则不通知组件断开连接，续活。
 */
- (void)continueLive{
    
    if ([self isConnected]) {
        return;
    }
    
    [self didVoipDisConnected];
    
    [self stopContinueLiveTimer];
    
}

//断开链接
- (void)didVoipDisConnected{
    _isOnline = NO;
    
    SoftphoneManager * delegate =  [SoftphoneManager instance];
    
    if ([delegate respondsToSelector:@selector(uxOnDisconnect)]) {
        [delegate performSelector:@selector(uxOnDisconnect)];
    }
    
}








//TCP模块错误描述
-(void)TCPConDidFailToCode:(NSInteger)code WithDescription:(NSString *)descript
{
    [[SDKLogManager instance]saveSDKLogInfo:[NSString stringWithFormat:@"TCP模块错误描述:%ld",code] withDetail:descript];
    
    //    if (nil != _backDelegate && [_backDelegate respondsToSelector:@selector(onConnectionFailed:)])
    //    {
    //        [_backDelegate performSelector:@selector(onConnectionFailed:) withObject:code];
    //    }
    //连接云平台错误码上报
    
    [[UCSStatiRequestManager instance] reportErrorCodeInfoForStati:@"connect:" withinterfaceDesc:@"连接云平台" withErrorCode:[NSString stringWithFormat:@"%ld",code]withErrorDesc:descript withClientNumber:[UserDefaultManager GetUserID]];
    
    
    //tcp连接出现异常则挂断当电通话
//    [[SoftphoneManager instance] hangUpCall];
    
}


#pragma mark - 拉取rtpp 和 cps 与上报数据

//上报数据与拉取cps WLS
- (void)reportPhoneInfoAndGetCPSParameter{
    
    //---------------------------------------------登陆成功后再拉取rtpp add by WLS------------------------------//
    [[UCSNetStaticInterface shareInstance] getTheSvrAddr];
    
    
    
    
    
    
    NSString * isreportuid = [UserDefaultManager GetKeyChain:DATA_STORE_ISREPORTUID];
    NSString * currentClientNumber = [UserDefaultManager GetclientNumber];
    
    
    if (isreportuid==nil) {//第一次登录的时候
        
        //上报数据
        
        
        [[UCSStatiRequestManager instance] reportPhoneInfoForStati:currentClientNumber];
        
        
        //保存当前userId
        [UserDefaultManager SetKeyChain:currentClientNumber key:DATA_STORE_ISREPORTUID];
        
    }else{
        
        
        if (![isreportuid isEqualToString:currentClientNumber]) {
            
            //上报数据
            
            [[UCSStatiRequestManager instance] reportPhoneInfoForStati:currentClientNumber];
            
            
            
            //保存当前userId
            [UserDefaultManager SetKeyChain:currentClientNumber key:DATA_STORE_ISREPORTUID];
            
        }
        
        
    }
    
    
    
    //动态策略服务参数获取，add by WLS 20150708
    
    //获取cps参数然后保存
    
    [[UCSCPSManager instance] getCPSParameter];
    
}



#pragma mark - 前后台通知 add by WLS 20151208

//- (void)enterBackground
//{
//
//    /**
//     @author WLS, 15-12-08 18:12:00
//     
//     当系统进入后台的时候，需要将视频采集关闭，不然会出现崩溃
//     */
//    [VideoView enableRender:FALSE];
//    [[CallLogManager instance] saveCallLogInfo:@"程序进入后台" withDetail:@"绘图关闭"];
//
//}

//- (void)enterForeground
//{
//    /**
//     @author WLS, 15-12-08 18:12:23
//     
//     当系统进入前台的时候，需要将视频采集打开
//
//     */
//    [VideoView enableRender:TRUE];
//    [[CallLogManager instance] saveCallLogInfo:@"程序进入前台" withDetail:@"绘图开启"];
//}


#pragma mark - tcp回调  add by WLS 20150818


//保存tcp返回的用户信息数据 WLS
- (void)saveUserInformation:(NSString *)userId andLoginType:(UCSTCPLoginType)loginType andClientNumber:(NSString *)clientNumber andPhone:(NSString *)phone andSSID:(NSString *)SSID andAppid:(NSString *)appid{
    
    //存储userId
    [UserDefaultManager SetKeyChain:userId key:DATA_STORE_ID];
    //存储appId
    [UserDefaultManager SetKeyChain:appid key:DATA_STORE_APPID];
    
    //存储登陆类型
    if (loginType == KCT_ClientLogin) {
        [UserDefaultManager SetKeyChain:@"1" key:DATA_STORE_ATYPE];
        
    }else if (loginType == KCT_TokenLogin){
        [UserDefaultManager SetKeyChain:@"0" key:DATA_STORE_ATYPE];
        
    }
    //存储clientNumber
    [UserDefaultManager SetKeyChain:clientNumber key:DATA_STORE_CLIENTNUMBER];
    //存储手机号
    [UserDefaultManager SetKeyChain:phone key:DATA_STORE_MOBILE];
    
    
    [UserDefaultManager SetKeyChain:SSID key:DATA_STORE_IM_SSID];
    
    //回拨时候会讲im_SSID是放入请求头中 WLS
    [UserDefaultManager SetIMSSID:SSID];
    
}

//tcp链接成功
- (void)didVoipConnectServerSuccessWithLoginInfo:(NSDictionary *)infoDic{
    
    
    int loginType = [[infoDic objectForKey:@"TcpLoginType"]intValue]; //登陆类型 旧账号登陆为1   新账号登陆为0
    
    NSString * phone = [infoDic objectForKey:@"TcpLoginPhone"]; //绑定的手机号
    NSString * clientNumber = [infoDic objectForKey:@"TcpLoginClientNumber"];//clientNumber;
    NSString * userId = [infoDic objectForKey:@"TcpLoginUserid"];
    NSString * clientPwd =[infoDic objectForKey:@"TcpLoginClientPwd"];
    NSString * token = [infoDic objectForKey:@"TcpLoginToken"];
    NSString * appId = [infoDic objectForKey:@"TcpLoginAppID"];
    
    NSString * SSID = nil;
    if (loginType == KCT_TokenLogin) {
        //token登陆 新账号登陆体系  用于回拨的ssid参数
        SSID  = token;
    }else{
        //旧账号登陆体系
        SSID = clientPwd;
        
    }
    
    
    
    //存储用户信息 WLS
    [self saveUserInformation:userId andLoginType:loginType andClientNumber:clientNumber andPhone:phone andSSID:SSID andAppid:appId];
    
    
    
    self.isRelogin = NO;
    [self ConnectedSuccess];
    
    //上报数据与拉取cps WLS
    [self reportPhoneInfoAndGetCPSParameter];
    
}



//接收数据回调
- (void)didVoipReceiveMessage:(KCTError *)error withData:(NSData *)reciveData{
    if (error.code == ErrorCode_NoError) {
        
        //收到数据包之后发给组件 WLS
        if ([[SoftphoneManager instance] respondsToSelector:@selector(uxOnRecived:)]) {
            [[SoftphoneManager instance] performSelector:@selector(uxOnRecived:) withObject:reciveData];
        }
        
        
    }
}


/*!
 * add by WLS 20151117
 *  @brief  连接状态变化时
 *
 *  @param connectionStatus 连接状态
 *  @param error            错误描述
 */
- (void)didConnectionStatusChanged:(NSString *)statusStr  error:(KCTError *) error{
    
    int connectionStatus = [statusStr intValue];
    switch (connectionStatus) {
        case KCTConnectionStatus_loginSuccess:
            //登陆成功
        {
            
        }
            break;
        case KCTConnectionStatus_ReConnectSuccess:
        {
            //重连成功
            //tcp链接成功之后 更新组件的链接状态
            [[SDKLogManager instance]saveSDKLogInfo:@"tcp模块重连成功" withDetail:@"tcp重连成功"];
            
            
            self.isRelogin = YES;
            
            [self ConnectedSuccess];
            
            //上报数据与拉取cps WLS
            [self reportPhoneInfoAndGetCPSParameter];
        }
            break;
        case KCTConnectionStatus_ConnectFail:
            //登陆失败
            break;
        case KCTConnectionStatus_SignOut:
        {
            //退出登录，主动退出登录
            
        }
        case KCTConnectionStatus_AbnormalDisconnection:
            //异常断开
        {
//            [self didVoipDisConnected];
            
            if (connectionStatus == KCTConnectionStatus_AbnormalDisconnection) {
                if (_continueLiveTimer) {
                    
                }else{
                    _continueLiveTimer = [[NSTimer scheduledTimerWithTimeInterval:UCSContinueLive target:self selector:@selector(continueLive) userInfo:nil repeats:NO] retain];
                    [[NSRunLoop currentRunLoop] addTimer:_continueLiveTimer forMode:NSDefaultRunLoopMode];
                }
            }else{
                /**
                 @author WLS, 16-05-04 12:05:47
                 
                 退出登录，主动断开
                 */
                [self didVoipDisConnected];
                [self stopContinueLiveTimer];

            }
            
            [self TCPConDidFailToCode:error.code WithDescription:error.errorDescription];
            
            //链接断开 停止拉取rtpp
            [[UCSNetStaticInterface shareInstance]  stopGetRtpp];
            
            //添加异常断开保存  add by WLS 20151026
            [[SDKLogManager instance]saveSDKLogInfo:@"tcp模块要断开啦" withDetail:@"tcp断开"];
        }
            break;
        case KCTConnectionStatus_StartReConnect:
        {
            //开始重连
            [[SDKLogManager instance]saveSDKLogInfo:@"tcp模块要重连" withDetail:@"tcp重连"];
        }
            break;
        case KCTConnectionStatus_ReConnecting:
        {
            //正在重连
            [[SDKLogManager instance]saveSDKLogInfo:@"tcp模块正在重连" withDetail:@"tcp重连"];
        }
            break;
            
        case KCTConnectionStatus_ReConnectFail:
            //重连失败
            break;
        case KCTConnectionStatus_BeClicked:
            //账号在别处登录,被强行下线
        {
            
            [self didVoipDisConnected];
            
            
            [self TCPConDidFailToCode:error.code WithDescription:error.errorDescription];
            
            //链接断开 停止拉取rtpp
            [[UCSNetStaticInterface shareInstance]  stopGetRtpp];
            
            //添加异常断开保存  add by WLS 20151026
            [[SDKLogManager instance]saveSDKLogInfo:@"被踢线啦" withDetail:@"tcp断开"];
            
            
        }
            break;
            
        default:
            break;
    }
    
}









//tcp消息发送失败
- (void)tcpSendFailedWithMessage:(KCTMessageFrame *)messageFrame andError:(KCTError *)error{
    
    if (error == nil) {
        //说明此时tcp处于链接状态 此时重新发包
        
        [self send:messageFrame.message];
        
    }else{
        
        //此时tcp异常 不能发包
        [self TCPConDidFailToCode:error.code WithDescription:error.errorDescription];
        [[SoftphoneManager instance] hangUpCall];
    }
    
    
}






@end
