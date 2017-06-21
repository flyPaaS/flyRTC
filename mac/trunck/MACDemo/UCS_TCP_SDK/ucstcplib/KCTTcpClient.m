//
//  KCTTcpClient.m
//  Tcplib
//
//  Created by FredZhang on 15-4-11.
//  Copyright (c) 2016年 KCTMAC. All rights reserved.
// c

#import "KCTTcpClient.h"
#import "UCSTCPManager.h"
#import "KCTTcpDefine.h"
#import "OffLinePushManager.h"
#import "UCSSendMessageQueue.h"
#import "UCSTCPSdkDataReporter.h"
#import "UCSTCPProxyManager.h"
#import "UCSTCPConst.h"
#import "UCSTCPDevice.h"
#import "UCSTCPTransParentConvert.h"
#import "UCSTCPTimer.h"

#import <ifaddrs.h>
#import <arpa/inet.h>
#import <net/if.h>

#import <CommonCrypto/CommonCryptor.h>
#import <CommonCrypto/CommonDigest.h>



typedef NS_ENUM(NSUInteger, UCSTCPConnectionType) {
    UCSTCPConnectionType_IM = 1,  //IM
    UCSTCPConnectionType_Voip = 2, //voip
    UCSTCPConnectionType_CorporateCommunications = 3, //企业通信
    UCSTCPConnectionType_TransParent = 4, //公共透传
};


//登陆参数key宏定义
#define KTcpLoginToken         @"TcpLoginToken"
#define KTcpLoginAccountSid    @"TcpLoginAccountSid"
#define KTcpLoginAccountToken  @"TcpLoginAccountToken"
#define KTcpLoginClientNumber  @"TcpLoginClientNumber"
#define KTcpLoginClientPwd     @"TcpLoginClientPwd"
#define KTcpLoginType          @"TcpLoginType"
#define KTcpLoginUserId        @"TcpLoginUserid"
#define KTcpLoginAppID         @"TcpLoginAppID"
#define KTcpLoginPhone         @"TcpLoginPhone"

//add by wenqinglin
#define kKC_im_ssid        @"KC_im_ssid"
#define kKC_ssid           @"KC_ssid"
#define kKC_uid            @"KC_uid"
#define kKC_phone          @"KC_phone"
#define kKC_name           @"KC_name"
#define kKC_CsUrl          @"KC_csUrl"
#define kKC_RtppUrl        @"KC_rtppUrl"


#define KUCSTCP_TARGET_IPHONE_SIMULATOR  [[UCSTCPDevice alloc] init].issSimulator


typedef void (^loginSuccessBlock)(NSString *userId);
typedef void (^loginErrorBlock)(KCTError *error);

@interface KCTTcpClient()

@property (nonatomic, assign)UCSTCPLoginType login_type;
@property (nonatomic, strong)NSMutableArray * completionBlocks;

@property (nonatomic, copy) loginSuccessBlock    aloginSuccessBlock;
@property (nonatomic, copy) loginErrorBlock      aloginErrorBlock;

@property (nonatomic, strong) UCSTCPTimer * loginTimeoutTimer;  //登陆定时器
@property (nonatomic, strong) UCSTCPTimer * transParentTimer; //发送透传数据定时器

@property (nonatomic, strong) id   imClient;  //imClient
@property (nonatomic, strong) id   voipClient; //voipClient

@property (nonatomic, assign) id<KCTTCPDelegateBase> tcpDeleagte; //tcp代理对象

@property (nonatomic, assign) int   isDev;
@property (nonatomic, assign) KCTPushEnvironment environment; //推送环境

@property (nonatomic, copy) NSString * appid; //KCT  appid

@property (nonatomic, assign) BOOL isServiceInit;
@property (nonatomic, strong) NSNumber * ssid; //kc ssid
@property (nonatomic, strong) NSString * im_ssid; //kc im_ssid

@end

@implementation KCTTcpClient

static id _instace;
+(instancetype)sharedTcpClientManager
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instace = [[self alloc] init];
    });
    return _instace;
}

+(id)allocWithZone:(struct _NSZone *)zone
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instace=[super allocWithZone:zone];
    });
    return _instace;
}

-(id)copyWithZone:(struct _NSZone *)zone
{
    return _instace;
}


#pragma mark getter
- (NSString *)version{
    return KTcpSDKVersion;
}

- (NSMutableArray *)completionBlocks{
    if (_completionBlocks == nil) {
        _completionBlocks = [NSMutableArray array];
    }
    return _completionBlocks;
}

-(UCSTCPTimer *)loginTimeoutTimer{
    if (_loginTimeoutTimer == nil) {
        _loginTimeoutTimer = [UCSTCPTimer timerWithTimeInterval:30.0 target:self selector:@selector(loginTimeout) repeats:NO];
    }
    return _loginTimeoutTimer;
}

- (UCSTCPTimer *)transParentTimer{
    if (_transParentTimer == nil) {
        _transParentTimer = [UCSTCPTimer timerWithTimeInterval:30.0 target:self selector:@selector(sendTransParentDataTimeOut) repeats:YES];
    }
    return _transParentTimer;
}

/*!
 *  @brief  释放登陆定时器
 */
- (void)releaseLoginTimer{
    if (_loginTimeoutTimer) {
        [_loginTimeoutTimer invalidate];
        _loginTimeoutTimer = nil;
    }
}
/**
 *  @brief 释放透传定时器
 */
- (void)releaseTransParentTimer{
    if (_transParentTimer) {
        [_transParentTimer invalidate];
        _transParentTimer = nil;
    }
}


#pragma mark  ---- setter

/*!
 *  @author barry, 15-11-13 11:11:43
 *
 *  @brief  设置tcp代理对象
 */
- (void)setTcpDelegate:(id<KCTTCPDelegateBase>)delegate{
    [UCSTCPManager instance].tcpDelegate = delegate;
    _tcpDeleagte = delegate;
    
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"设置tcp代理"];
}


/*!
 *  @author barry, 15-11-13 11:11:49
 *
 *  @brief  设置 IMclient
 */
- (void)setImClient:(id)imClient{
    [UCSTCPManager instance].imClient = imClient;
    _imClient = imClient;
    
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"设置im代理"];
}



/*!
 *  @author barry, 15-11-13 11:11:33
 *
 *  @brief  设置voipclient
 */
- (void)setVoipClient:(id)voipClient{
    [UCSTCPManager instance].voipClient = voipClient;
    _voipClient = voipClient;
    
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"设置voip代理"];
}


- (void)setPushEnvironment:(KCTPushEnvironment)environment deviceToken:(NSData *)deviceToken{
    if (deviceToken.length <= 0 || KUCSTCP_TARGET_IPHONE_SIMULATOR) {
        return;
    }
    
    self.environment = environment;
    /*
    NSString *ucsDeviceToken = [[[[deviceToken description]
                                stringByReplacingOccurrencesOfString:@"<"withString:@""]
                               stringByReplacingOccurrencesOfString:@">" withString:@""]
                              stringByReplacingOccurrencesOfString: @" " withString: @""];*/
    [[NSUserDefaults standardUserDefaults] setObject:deviceToken forKey:@"UCSDeviceToken"];
    [[NSUserDefaults standardUserDefaults] synchronize];
    
    //[[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:[NSString stringWithFormat:@"设置推送环境%d,设置deviceToken:%@, ucsDeviceToken:%@", environment, deviceToken, ucsDeviceToken]];
}


-(id)init{
    self = [super init];
    if (self) {
        
        self.isServiceInit = NO;
        
        //监听 登陆回执
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(login_Response:) name:kNotiResponse_Login_1010 object:nil];
        
        //监听 连接即将断开
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(connectionWillDisConnectNotification:) name:kConnectionWillDisConnectNotification object:nil];
        
        //监听 重连请求
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(relogin_Resquest:) name:kReLogin_Request_Notification object:nil];
        
        //监听 重连回执
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(relogin_Response:) name:kNotiResponse_reLogin_1020 object:nil];
        
        
        //监听 tcp握手失败事件
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(tcpHandleFailWithLogin:) name:KTcpHandleFailWhenLogin object:nil];
        
        //监听 tcp连接被T事件
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(tcpErrorCodeBeClicked:) name:KTcpErrorCodeBeClicked object:nil];
        
        //监听 切换ip登陆请求
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(connect) name:kConnectWithNextIpNotification object:nil];
        
        //监听 重连失败通知
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(callBackReConnectFail) name:kReconnectFailNotification object:nil];
        
        //监听 发送透传请求的回执
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(sendTransParentDataResponse:) name:UCSTcpSendTransParentDataResponseNotification object:nil];
        
        //监听 收到透传请求
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didReceiveTransParentData:) name:UCSTcpDidReceiveTransParentDataNotification object:nil];
        
        //add by wenqinglin 20170106
        //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(serviceInitSuccessful) name:UCSNotiEngineSucess object:nil];
        
        //监控网络
        //[UCSTCPGLobalRealReachability startMonitoring];
        
    }
    
    return self;
}


- (void) dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [self releaseLoginTimer];
    [self releaseTransParentTimer];
    
    [self.completionBlocks removeAllObjects];
    self.completionBlocks = nil;
}


//- (void)serviceInitSuccessful {
//    self.isServiceInit = YES;
//    [[NSNotificationCenter defaultCenter] removeObserver:self name:UCSNotiEngineSucess object:nil];
//}

#define IOS_CELLULAR    @"pdp_ip0"
#define IOS_WIFI        @"en0"
#define IOS_VPN         @"utun0"
#define IP_ADDR_IPv4    @"ipv4"
#define IP_ADDR_IPv6    @"ipv6"

- (BOOL)isValidatIP:(NSString *)ipAddress {
    if (ipAddress.length == 0) {
        return NO;
    }
    NSString *urlRegEx = @"^([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\."
    "([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\."
    "([01]?\\d\\d?|2[0-4]\\d|25[0-5])\\."
    "([01]?\\d\\d?|2[0-4]\\d|25[0-5])$";
    
    NSError *error;
    NSRegularExpression *regex = [NSRegularExpression regularExpressionWithPattern:urlRegEx options:0 error:&error];
    
    if (regex != nil) {
        NSTextCheckingResult *firstMatch=[regex firstMatchInString:ipAddress options:0 range:NSMakeRange(0, [ipAddress length])];
        
        if (firstMatch) {
            NSRange resultRange = [firstMatch rangeAtIndex:0];
            NSString *result=[ipAddress substringWithRange:resultRange];
            //输出结果
            //NSLog(@"%@",result);
            return YES;
        }
    }
    return NO;
}

- (NSDictionary *)getIPAddresses
{
    NSMutableDictionary *addresses = [NSMutableDictionary dictionaryWithCapacity:8];
    
    // retrieve the current interfaces - returns 0 on success
    struct ifaddrs *interfaces;
    if(!getifaddrs(&interfaces)) {
        // Loop through linked list of interfaces
        struct ifaddrs *interface;
        for(interface=interfaces; interface; interface=interface->ifa_next) {
            //printf("net:--------%s--\n",interface->ifa_name);
            if(!(interface->ifa_flags & IFF_UP) /* || (interface->ifa_flags & IFF_LOOPBACK) */ ) {
                continue; // deeply nested code harder to read
            }
            const struct sockaddr_in *addr = (const struct sockaddr_in*)interface->ifa_addr;
            char addrBuf[ MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN) ];
            if(addr && (addr->sin_family==AF_INET || addr->sin_family==AF_INET6)) {
                NSString *name = [NSString stringWithUTF8String:interface->ifa_name];
                NSString *type;
                if(addr->sin_family == AF_INET) {
                    if(inet_ntop(AF_INET, &addr->sin_addr, addrBuf, INET_ADDRSTRLEN)) {
                        type = IP_ADDR_IPv4;
                    }
                } else {
                    const struct sockaddr_in6 *addr6 = (const struct sockaddr_in6*)interface->ifa_addr;
                    if(inet_ntop(AF_INET6, &addr6->sin6_addr, addrBuf, INET6_ADDRSTRLEN)) {
                        type = IP_ADDR_IPv6;
                    }
                }
                if(type) {
                    NSString *key = [NSString stringWithFormat:@"%@/%@", name, type];
                    addresses[key] = [NSString stringWithUTF8String:addrBuf];
                }
            }
        }
        // Free memory
        freeifaddrs(interfaces);
    }
    return [addresses count] ? addresses : nil;
}

#pragma mark - 获取设备当前网络IP地址
- (NSString *)getIPAddress:(BOOL)preferIPv4
{
    NSArray *searchArray = preferIPv4 ?
    @[ IOS_VPN @"/" IP_ADDR_IPv4, IOS_VPN @"/" IP_ADDR_IPv6, IOS_WIFI @"/" IP_ADDR_IPv4, IOS_WIFI @"/" IP_ADDR_IPv6, IOS_CELLULAR @"/" IP_ADDR_IPv4, IOS_CELLULAR @"/" IP_ADDR_IPv6 ] :
    @[ IOS_VPN @"/" IP_ADDR_IPv6, IOS_VPN @"/" IP_ADDR_IPv4, IOS_WIFI @"/" IP_ADDR_IPv6, IOS_WIFI @"/" IP_ADDR_IPv4, IOS_CELLULAR @"/" IP_ADDR_IPv6, IOS_CELLULAR @"/" IP_ADDR_IPv4 ] ;
    
    NSDictionary *addresses = [self getIPAddresses];
    
    __block NSString *address;
    [searchArray enumerateObjectsUsingBlock:^(NSString *key, NSUInteger idx, BOOL *stop)
     {
         address = addresses[key];
         //筛选出IP地址格式
         if([self isValidatIP:address]) *stop = YES;
     } ];
    return address ? address : @"0.0.0.0";
}


#pragma mark 连接平台


- (NSString *) md5String:(NSString *)str {
    const char *cStr = [str UTF8String];
    unsigned char result[16];
    CC_MD5( cStr, strlen(cStr), result );
    return [[NSString stringWithFormat:
             @"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
             result[0], result[1], result[2], result[3],
             result[4], result[5], result[6], result[7],
             result[8], result[9], result[10], result[11],
             result[12], result[13], result[14], result[15]
             ] lowercaseString];
}



#ifdef kLocalAreaNetworkEnvironment
#define passURL @"http://192.168.0.8:8097/ams2/login.act?"
#else
#define passURL @"http://59.110.10.28:8097/ams2/login.act?"
#endif


#define kMd5EncodeKey @"~U!X@I#N$"


//add by wenqinglin
//明文登录方式：
- (void)connect:(NSString *)accountSid accountToken:(NSString *)accountToken clientNumber:(NSString *)clientNumber clientPwd:(NSString *)clientPwd success:(loginSuccessBlock)success failure:(loginErrorBlock)failure {
    
    self.aloginSuccessBlock = success;
    self.aloginErrorBlock = failure;
    
    
    NSString *sid = [NSString stringWithFormat:@"%@%@",accountSid,kMd5EncodeKey];
    NSString *token = [NSString stringWithFormat:@"%@%@",accountToken,kMd5EncodeKey];
    NSString *pwd = [NSString stringWithFormat:@"%@%@",clientPwd,kMd5EncodeKey];
    
    accountSid = [self md5String:sid];
    accountToken = [self md5String:token];
    clientPwd = [self md5String:pwd];
    
    NSString * asAddress = [self getIPAddress:YES];
    NSString * loginUrl = [NSString stringWithFormat:@"%@account=%@&accounttype=mobile&accountpwd=%@&ip=%@&pv=ios&imei=123456&securityver=0&mainaccount=%@&mainaccountpwd=%@",passURL,clientNumber,clientPwd,asAddress,accountSid,accountToken];
    //printf("-----url is : %s",[loginUrl UTF8String]);
    //arms 认证
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:loginUrl]];
    request.HTTPMethod = @"GET";
    
    __weak typeof(self) weakSelf = self;
    [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue mainQueue] completionHandler:^(NSURLResponse * _Nullable response, NSData * _Nullable data, NSError * _Nullable connectionError) {
        
        if (data) {
            NSDictionary * dict = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
            if ([[dict valueForKey:@"result"] intValue] == 0 && [dict valueForKey:@"result"]) {
                //[[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:[NSString stringWithFormat:@"arms 认证成功：%@",dict]];
                /**
                 @author WLS, 16-04-05 15:04:05
                 
                 获取单向外呼免费通话分钟数
                 */
                //[[KCTVOIPViewEngine getInstance] getCallBalance];
                NSString *tIm_ssid = [dict objectForKey:@"im_ssid"];
                NSString *tPhone = [dict objectForKey:@"phone"];
                NSNumber *tSsid = [dict objectForKey:@"ssid"];
                NSNumber *tUid = [dict objectForKey:@"uid"];
                NSString *tName = [dict objectForKey:@"name"];
                //保存token
                NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
                
                if (tIm_ssid) {
                    weakSelf.im_ssid = tIm_ssid;
                    [defaults setObject:tIm_ssid forKey:kKC_im_ssid];
                }
                if (tPhone) {
                    [defaults setObject:tPhone forKey:kKC_phone];
                }
                if (tSsid) {
                    weakSelf.ssid = tSsid;
                    [defaults setObject:tSsid forKey:kKC_ssid];
                }
                if (tUid) {
                    [defaults setObject:tUid forKey:kKC_uid];
                }
                
                [defaults setInteger:KCT_ClientLogin forKey:KTcpLoginType];
                [defaults synchronize];
                
                
                [defaults setObject:tName forKey:kKC_name];
                [defaults synchronize];
                
                
                if (tUid) {
                    NSString *tClientNumber = tUid.description;
                    [weakSelf getCSAndRtppUrl:tClientNumber];
                 }
                 //连接云平台
                //[[KCTTcpClient sharedTcpClientManager] login_connect];
                
                
            }else{
                [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:[NSString stringWithFormat:@"arms 认证失败：%@",dict]];
            }
        } else {
            [[NSNotificationCenter defaultCenter] postNotificationName:TCPConnectStateNotification object:KCTCPNoNetWorkNotification];
            [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:[NSString stringWithFormat:@"arms 认证失败：%@",connectionError]];
        }
    }];
    
}

#ifdef kLocalAreaNetworkEnvironment
#define csAndRtppURL @"http://192.168.0.8:8097/v3/geturl?"
#else
#define csAndRtppURL @"http://59.110.10.28:8097/v3/geturl?"
#endif

- (void)getCSAndRtppUrl:(NSString *)clientNum
{
    NSString *url = [NSString stringWithFormat:@"%@clientnum=%@",csAndRtppURL,clientNum];
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]];
    request.HTTPMethod = @"GET";
    [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue mainQueue] completionHandler:^(NSURLResponse * _Nullable response, NSData * _Nullable data, NSError * _Nullable connectionError) {
        NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
        if (data) {
            NSDictionary * dict = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
            if (dict) {
                NSString *csUrl = [dict objectForKey:@"getcsurl"];
                NSString *rtppUrl = [dict objectForKey:@"getrtppurl"];
                [defaults setObject:csUrl forKey:kKC_CsUrl];
                [defaults setObject:rtppUrl forKey:kKC_RtppUrl];
            }
        }
        else
        {
            [defaults setObject:nil forKey:kKC_CsUrl];
            [defaults setObject:nil forKey:kKC_RtppUrl];
        }
        [defaults synchronize];
        //连接云平台
        [[KCTTcpClient sharedTcpClientManager] login_connect];
    }];
}

//密文登录方式：
- (void)connect:(NSString *)token {
    if (self.isServiceInit) {
        
    } else {
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"Service初始化未成功，请稍后再登录"];
    }
}
/*!
 *  @brief  tcp登陆 。 token登陆 new
 */
- (void)login_connect{
    
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"开始 连接 cs 服务器"];
    
    Byte bytes[] = {0,0,0,0x20};
    NSData *data =  [NSData dataWithBytes:bytes length:4];
    
    
    //登陆之前先断开一下
    [[UCSTCPManager instance] disconnectServer:NO];
    
    //定时器开始登陆超时判断
    self.loginTimeoutTimer;
    
    //设置登陆数据
    [UCSTCPManager instance].loginData = data;
    
    //开始连接
    [self connect];
}


/**
 *  处理失败block
 *  @param error error
 */
- (void)handleErrorBlock:(KCTError *)error{
    if (self.aloginErrorBlock) {
        self.aloginErrorBlock(error);
        self.aloginErrorBlock = nil;
    }
    if (self.aloginSuccessBlock) {
        self.aloginSuccessBlock = nil;
    }
    
}
/**
 *  处理成功block
 *  @param userid 登陆返回id
 */
- (void)handleSuccessBlock:(NSString *)userid{
    if (self.aloginSuccessBlock) {
        self.aloginSuccessBlock(userid);
        self.aloginSuccessBlock = nil;
    }
    if (self.aloginErrorBlock) {
        self.aloginErrorBlock = nil;
    }
    
}






/*!
 *  @brief  查询tcp连接状态
 *
 *  @return yes 连接中， no 断开
 */
- (BOOL)login_isConnected
{
   return  [UCSTCPRealConnecter sharedInstance].realConnected;
}

- (KCTNetworkStatus)getCurrentNetWorkStatus
{
    return KCTReachableViaWWAN;
}


/*!
 *  @brief  断开连接。
 *
 *  @param flag    yes,关闭推送； no,不关闭
 */
- (void)login_uninitWithFlag:(BOOL)flag{
    
    [self releaseLoginTimer];
    [self releaseTransParentTimer];
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSNumber *numUid = [defaults objectForKey:kKC_uid];
    NSString *tClientNumber = numUid.description;
    
    
    [self.completionBlocks removeAllObjects];
    
    if (flag  && !KUCSTCP_TARGET_IPHONE_SIMULATOR ){
        
        //真机，注销离线推送
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"断开连接,flag=yes,关闭推送"];
        //注销
        [OffLinePushManager closeOffLinePushWithEnvironment:self.environment clientId:tClientNumber];
        //将推送标识置为yes
        [UCSTCPManager instance].offlinePushClosed = YES;
        
    }else{
        
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"断开连接,flag=no,不关闭推送"];
        //广播tcp主动断开
        [[NSNotificationCenter defaultCenter] postNotificationName:UCSTCPDisconnectTcpConnectionActiveNotification object:nil];
        
        
    }
    [[UCSTCPManager instance] sendLogoutPack];
    [[UCSTCPManager instance] disconnectServer:YES];
    
}

- (void)login_uninitWithFlag2:(BOOL)flag{
    
    [self releaseLoginTimer];
    [self releaseTransParentTimer];
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSNumber *numUid = [defaults objectForKey:kKC_uid];
    NSString *tClientNumber = numUid.description;
    
    [self.completionBlocks removeAllObjects];
    
    if (flag  && !KUCSTCP_TARGET_IPHONE_SIMULATOR ){
        
        //真机，注销离线推送
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"断开连接,flag=yes,关闭推送"];
        //注销
        [OffLinePushManager closeOffLinePushWithEnvironment:self.environment clientId:tClientNumber];
        //将推送标识置为yes
        [UCSTCPManager instance].offlinePushClosed = YES;
        
    }else{
        
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"断开连接,flag=no,不关闭推送"];
        //广播tcp主动断开
        [[NSNotificationCenter defaultCenter] postNotificationName:UCSTCPDisconnectTcpConnectionActiveNotification object:nil];
        
        [[UCSTCPManager instance] disconnectServer:YES];
    }
    
}



#pragma mark 透传

/**
 *  @brief 发送透传请求
 */
- (KCTTCPTransParentRequest *)sendTransParentData:(KCTTCPTransParentRequest *)request success:(void (^)(KCTTCPTransParentRequest *))success failure:(void (^)(KCTTCPTransParentRequest *, KCTError *))failure{
    
    request.requestIdentifier = [UCSTCPTransParentConvert requestIdentifier];
    
    if (success && failure) {
        //block是否需要拷贝？？？
        __weak __typeof(self)weakSelf = self;
        @synchronized (self) {
            UCSTCPTransParentRequestBlockObj * blockObj = [[UCSTCPTransParentRequestBlockObj alloc] init];
            blockObj.request = request;
            blockObj.success = success;
            blockObj.failure = failure;
            blockObj.time = time(NULL);
            [weakSelf.completionBlocks addObject:blockObj];
        }
    }
    
    UCSTCPTransParentSubRequest *subRequest = [[UCSTCPTransParentSubRequest alloc] init];
    subRequest.appid = self.appid;
    subRequest.requestIdentifier = request.requestIdentifier;
    subRequest.senderId = [UCSTCPTransParentConvert senderUserId];
    subRequest.receiveId = request.receiveId;
    subRequest.cmdString = request.cmdString;
    
    self.transParentTimer;
    
    NSData *data = [UCSTCPTransParentConvert convertTransParentRequest2Data:subRequest];
    
    
    if (request.cmdString.length >256 || request.cmdString.length==0) {
        
        /**
         @author WLS, 16-05-26 18:50:40
         
         如果发送的长度为空或者超过256字节的时候，直接返回错误
         */
        KCTError *error;
        
        if (request.cmdString.length>256) {
            error = [KCTError errorWithCode:ErrorCode_MessageLengthTooLong andDescription:@"透传数据内容过长"];
        }else{
            error = [KCTError errorWithCode:ErrorCode_EmptyMessage andDescription:@"透传数据为空"];
        }
        
        UCSTCPTransParentResponse *reponse = [[UCSTCPTransParentResponse alloc] init];
        reponse.error = error;
        reponse.requestIdentifier = request.requestIdentifier;
        
        dispatch_async(dispatch_get_main_queue(), ^(void){
            //广播透传响应
            [[NSNotificationCenter defaultCenter] postNotificationName:UCSTcpSendTransParentDataResponseNotification object:reponse];
        });

        
        return request;
    }
    
    [self sendData:data modelType:@4];
 
    return request;
    
}



#pragma mark private

- (void)sendData:(NSData *)data modelType:(NSNumber *)modelType{
    
    // 没网不发送
    if ([self getCurrentNetWorkStatus] == KCTNotReachable) {
        return;
    }
    
    
    if (UCSTCPConnectionType_IM == [modelType integerValue]) {
        
        //modify by wenqinglin
        [[UCSTCPManager instance] sendData:data];
        
    }else if(UCSTCPConnectionType_Voip == [modelType integerValue]){

        // voip处于登陆状态才可以发包
        if ([[UCSTCPManager instance] isClientOnline]) {
            
            if ([self login_isConnected]) {
                //如果处于链接状态 则发送消息 by WLS 20151027
                [[UCSTCPManager instance] sendData:data];
                
            }else{
                if ([UCSTCPManager instance].beingConnected) {
                    //如果处于链接过程中则不做操作 by WLS 20151027
                }else{
                    //如果不处于链接过程中则重连 by WLS 20151027
                    if ([[UCSTCPManager instance] respondsToSelector:@selector(reConnect)]) {
                        [[UCSTCPManager instance] performSelector:@selector(reConnect)];
                    }
                    
                }
                //此时处于重连状态，将发送的包体保存，等待重连成功后将未发送的包重发  by WLS 20151026
                [[UCSSendMessageQueue instance] addNewSendMessage:data andSendDelegate:_voipClient andRevivedNoti:NO];
            }
            
        }
        
        
    }else if (UCSTCPConnectionType_CorporateCommunications == [modelType integerValue]){
        
        
    }else if (UCSTCPConnectionType_TransParent == [modelType integerValue]){
        
        [[UCSTCPManager instance] sendData:data];
        
    }else{
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"发送消息传入的modelType不正确"];
    }
    
}



#pragma mark proxy
/**
 *  开始获取ip，发起连接
 */
- (void)connect{
    
    BOOL isLogged = [UCSTCPManager instance].isClientOnline;
    [[UCSTCPProxyManager sharedInstance] loadProxyWithHttp:isLogged iP:^(NSString *ip, BOOL end, NSError *error) {
        TCPWEAKSELF
        //获取ip失败,直接抛出握手失败
        if (error) {
            
            KCTError *error;
            if (end) {
                //end=yes。说明遍历了所有ip还是没成功，回调 连接服务器失败
                [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"end==yes,连接服务器失败"];
                error = [KCTError errorWithCode:ErrorCode_ConnectToServerFail andDescription:@"连接服务器失败"];
            }else{
                //end=no。说明本地没有ip并且请求ip失败，回调 请求ip失败
                [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"获取ip失败"];
                error = [KCTError errorWithCode:ErrorCode_ErrorPullProxy andDescription:@"请求ip列表失败"];
            }
            
            NSNotification *note = [NSNotification notificationWithName:KTcpHandleFailWhenLogin object:error];
            [weakSelf tcpHandleFailWithLogin:note];
            return ;
            
        }
////        ip = @"113.31.88.106:80";
//                ip = @"113.31.88.114:80";
        UCS_TCP_Log(@"connect ip:%@", ip);
        //连接tcp
        if (ip.length > 0) {
            [weakSelf connectToTcpWithIp:ip];
        }
        
    }];
}

//modify by wenqinglin for 切换网络的时候重连失败

- (void)reConnect2{
    
    BOOL isLogged = [UCSTCPManager instance].isClientOnline;
    [[UCSTCPProxyManager sharedInstance] loadProxyWithHttp:isLogged iP:^(NSString *ip, BOOL end, NSError *error) {
        TCPWEAKSELF
        //error存在，说明是重连所有本地ip后重新拉取proxy失败。并且在等待一会儿后这个block会自动回调，再次发起重连
        if (error) {
            [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"重连时,获取ip失败"];
            [weakSelf callBackReConnectFail];
            [weakSelf reConnect];
            return ;
        }
        
        //连接tcp
        UCS_TCP_Log(@"ReConnect ip:%@", ip);
        if (ip.length > 0) {
            [weakSelf connectToTcpWithIp:ip];
        }
    }];
}
/**
 *  开始重连
 */
- (void)reConnect{

    
    BOOL isLogged = [UCSTCPManager instance].isClientOnline;
    [[UCSTCPProxyManager sharedInstance] loadProxyWithHttp:isLogged iP:^(NSString *ip, BOOL end, NSError *error) {
        TCPWEAKSELF
        //error存在，说明是重连所有本地ip后重新拉取proxy失败。并且在等待一会儿后这个block会自动回调，再次发起重连
        if (error) {
            [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"重连时,获取ip失败"];
            [weakSelf callBackReConnectFail];
            [weakSelf reConnect];
            return ;
        }
        
        //连接tcp
        UCS_TCP_Log(@"ReConnect ip:%@", ip);
        if (ip.length > 0) {
            [weakSelf connectToTcpWithIp:ip];
        }
    }];
}

/**
 *  连接tcp
 *  @param ip ip地址
 */
- (void)connectToTcpWithIp:(NSString *)ip{
    //ip和port赋值
    NSArray * ipDic = [ip componentsSeparatedByString:@":"];
    [UCSTCPManager instance].ip = ipDic.firstObject;
    [UCSTCPManager instance].port = ipDic[1];
    
//    113.31.89.149:80    带voip的测试环境
//    113.31.88.114  不带voip的测试环境
//    113.31.88.106:80, 带voip的线上环境
//    172.16.12.96:80   开发环境，不稳定
    
    //tcp握手
    [[UCSTCPManager instance] connect2TCP];
}



#pragma mark notify


/*!
 *  @author barry, 15-10-19 15:10:55
 *
 *  @brief  收到tcp握手失败的通知,第一次连接才有，重连不会调用
 *
 *  @param note 通知
 */

- (void)tcpHandleFailWithLogin:(NSNotification *)note{
    
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"登陆时tcp握手失败"];
    
    [self releaseLoginTimer];
    
    KCTError *error = (KCTError *)note.object;
    [self handleErrorBlock:error];
    
    [self callBackConnectFailWithError:error];
}


/*!
 *  @brief  收到被踢下线的通知
 *
 *  @param note 通知
 */

- (void)tcpErrorCodeBeClicked:(NSNotification *)note
{
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"tcp被T"];
    
    //被T出时，取消同一ip的第二次连接，和重连拉取失败的再次连接
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    
    KCTError *error = (KCTError *)note.object;
    [self callBackBeClickedWithError:error];
}

-(void)login_Response:(NSNotification *) aNotification
{
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"登陆成功"];
    
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *phone = [defaults objectForKey:kKC_phone];
    NSNumber *numUid = [defaults objectForKey:kKC_uid];
    NSString *tUserName = [defaults objectForKey:kKC_name];
    NSString *tClientNumber = numUid.description;
    NSString *tBindMobile = phone;
    NSString *tAppid = @"aa2e2bc6e22e4fbfaf924fa75c02dc09";

    
    // 保存登陆id
    [defaults setObject:phone  forKey:@"loginUserId"];
    [defaults synchronize];
    
    //通知IMSDK数据库切换
    if (_imClient) {
        [[NSNotificationCenter defaultCenter] postNotificationName:@"UCSIMAccountChangedNotification" object:nil];
    }
    
    //回调登陆成功
    [self handleSuccessBlock:numUid.description];
    
    [[UCSTCPManager instance] onConnectSuccessfull];
    
    
    //组织登陆成功返回的数据
    UCSTCPLoginType type = (int)[defaults integerForKey:KTcpLoginType];
    NSDictionary * infodic ;
    
    if (type == KCT_ClientLogin) {
        
        //NSString * token = [defaults stringForKey:KTcpLoginToken];
        NSString *token = @"";
        infodic = @{KTcpLoginType :[NSString stringWithFormat:@"%d", KCT_TokenLogin],
                    KTcpLoginToken: token,
                    KTcpLoginUserId:tUserName,
                    KTcpLoginClientNumber:tClientNumber,
                    KTcpLoginPhone:tBindMobile,
                    KTcpLoginAppID :tAppid};
        
        
    }else if(type == KCT_TokenLogin){
        
        infodic = @{KTcpLoginType : [NSString stringWithFormat:@"%d", KCT_ClientLogin],
                    KTcpLoginAccountSid : @"",
                    KTcpLoginAccountToken : @"",
                    KTcpLoginClientNumber : tClientNumber,
                    KTcpLoginClientPwd : @"",
                    KTcpLoginPhone:tBindMobile,
                    KTcpLoginUserId:tUserName,
                    KTcpLoginAppID :tAppid};
        
    }else{}
    
    //appid赋值
    if (tAppid) {
        self.appid = [tAppid copy];
    }
    
    //运行时，通知imsdk登陆成功
    if (self.imClient && [self.imClient respondsToSelector:@selector(didconnectSuccessWithInfo:)]) {
        [self.imClient performSelector:@selector(didconnectSuccessWithInfo:) withObject:nil];
    }
    
    //通知voipsdk登陆成功
    if (self.voipClient && [self.voipClient respondsToSelector:@selector(didVoipConnectServerSuccessWithLoginInfo:)]) {
        [self.voipClient performSelector:@selector(didVoipConnectServerSuccessWithLoginInfo:) withObject:infodic];
    }
    
    //回调tcp代理，通知登陆成功
    [self callBackConnectSuccess];
    
    [UCSTCPRealConnecter sharedInstance].realConnected = YES;
    
    //sdk数据上报
    //[[UCSTCPSdkDataReporter sharedInstance] reportSdkData:aAuthResponse];
    
    if (!KUCSTCP_TARGET_IPHONE_SIMULATOR) {
        //打开推送
        [OffLinePushManager openOffLinePushWithEnvironment:self.environment clientId:tClientNumber];
    }
    
    return;
}


/*!
 *  @author barry, 15-10-08 15:10:02
 *
 *  @brief  收到tcp回调的错误
 *
 *  @param aNotification <#aNotification description#>
 */
-(void)connectionWillDisConnectNotification:(NSNotification*)aNotification{
    
    KCTError * error = (KCTError *)aNotification.object;
    
    //1、如果收到的错误是登陆时tcp握手失败，那么就取消登陆计时，回调登陆失败
    
    //2、其他就是tcp异常中断了
    
    if (error.code == ErrorCode_ConnectToServerFail) {
        
        if (_loginTimeoutTimer) {
            [self releaseLoginTimer];
            [self handleErrorBlock:error];
            [self callBackConnectFailWithError:error];
        }
        
    }else{
        
        [self callBackAbnormalDisconnectWithError:error];
        
    }
    
    
}




/*!
 *  @brief  收到重连通知，执行重连操作
 *
 *  @param aNotification notification
 */
-(void)relogin_Resquest:(NSNotification*)aNotification{
    
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"开始重连"];
    
    //回调开始重连
    [self callBackStartReConnect];
    
    
    Byte bytes[] = {0,0,0,0x20};
    NSData *data =  [NSData dataWithBytes:bytes length:4];
    [UCSTCPManager instance].reLoginData = data;
    
    //开始重连
    [self reConnect];
    
    
    //回调正在重连
    [self callBackReConnecting];
}



/*!
 *  @brief  收到重连结果通知
 *
 *  @param aNotification 通知
 */
-(void)relogin_Response:(NSNotification*)aNotification{
    
}




/**
 *  @brief 收到透传回执通知
 *
 *  @param note 通知
 */
- (void)sendTransParentDataResponse:(NSNotification *)note{
    UCSTCPTransParentResponse *response = (UCSTCPTransParentResponse *)note.object;
    
    KCTError *error = response.error;
    NSString *requestIdentifier = response.requestIdentifier;

    @synchronized (self) {
        [self.completionBlocks enumerateObjectsUsingBlock:^(UCSTCPTransParentRequestBlockObj *obj, NSUInteger idx, BOOL * _Nonnull stop) {
            if ([obj.request.requestIdentifier isEqualToString:requestIdentifier]) {
                if (error) {
                    obj.failure(obj.request, error);
                }else{
                    obj.success(obj.request);
                }
                [self.completionBlocks removeObject:obj];
            }
        }];
    }
    
    if (self.completionBlocks.count == 0) {
        [self releaseTransParentTimer];
    }
}

/**
 *  @brief 收到透传请求通知
 *
 *  @param note 通知
 */
- (void)didReceiveTransParentData:(NSNotification *)note{
    KCTTCPTransParent *parent = (KCTTCPTransParent *)note.object;
    if (self.tcpDeleagte && [self.tcpDeleagte respondsToSelector:@selector(didReceiveTransParentData:)]) {
        [self.tcpDeleagte didReceiveTransParentData:parent];
    }
    //响应透传请求
    NSData *data = [UCSTCPTransParentConvert convertTransParent2Data:parent];
    [self sendData:data modelType:@4];
}


#pragma mark 超时处理。

/*!
 *  @brief  登陆超时处理
 */
-(void)loginTimeout
{
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"登陆超时"];
    [self releaseLoginTimer];
    
    //清除socket连接
    [[UCSTCPManager instance]disconnectServer:YES];
    
    KCTError * error = [KCTError errorWithCode:ErrorCode_TIMEOUT andDescription:@"连接超时"];
    //登陆方法回调
    [self handleErrorBlock:error];
    //回调错误
    [self callBackConnectFailWithError:error];
    
}

/**
 *  @brief 透传超时
 */
- (void)sendTransParentDataTimeOut{
    
    @synchronized (self) {
        __weak __typeof(self)weakSelf = self;
        [self.completionBlocks enumerateObjectsUsingBlock:^(UCSTCPTransParentRequestBlockObj *obj, NSUInteger idx, BOOL * _Nonnull stop) {
            time_t currentTime = time(NULL);
            if ( (currentTime - obj.time) >= 30) {
                KCTError *error = [KCTError errorWithCode:ErrorCode_TIMEOUT andDescription:@"超时"];
                obj.failure(obj.request, error);
                [weakSelf.completionBlocks removeObject:obj];
            }
        }];
    }
    
    if (self.completionBlocks.count == 0) {
        [self releaseTransParentTimer];
    }
}



#pragma mark 回调代理


/*!
 *  @author barry, 15-11-12 15:11:06
 *
 *  @brief  回调连接成功
 */

- (void)callBackConnectSuccess{

    KCTError * error = [KCTError errorWithCode:ErrorCode_NoError andDescription:@"连接成功,没有错误"];
    
    if (self.tcpDeleagte && [self.tcpDeleagte respondsToSelector:@selector(didConnectionStatusChanged:error:)]) {
        [self.tcpDeleagte didConnectionStatusChanged:KCTConnectionStatus_loginSuccess error:error];
    }
    
     [self callBackConnectionStateToSDK:KCTConnectionStatus_loginSuccess error:error];

}


/*!
 *  @author barry, 15-11-12 15:11:43
 *
 *  @brief  回调连接失败
 */
- (void)callBackConnectFailWithError:(KCTError *)error{
    
    if (self.tcpDeleagte && [self.tcpDeleagte respondsToSelector:@selector(didConnectionStatusChanged:error:)]) {
        [self.tcpDeleagte didConnectionStatusChanged:KCTConnectionStatus_ConnectFail error:error];
    }
    
     [self callBackConnectionStateToSDK:KCTConnectionStatus_ConnectFail error:error];
    
}



/*!
 *  @author barry, 15-11-12 15:11:29
 *
 *  @brief  回调异常断开
 *
 *  @param error <#error description#>
 */
- (void)callBackAbnormalDisconnectWithError:(KCTError *) error {
    
    if (self.tcpDeleagte && [self.tcpDeleagte respondsToSelector:@selector(didConnectionStatusChanged:error:)]) {
        [self.tcpDeleagte didConnectionStatusChanged:KCTConnectionStatus_AbnormalDisconnection error:error];
    }
    
     [self callBackConnectionStateToSDK:KCTConnectionStatus_AbnormalDisconnection error:error];
}

/*!
 *  @author barry, 15-11-12 15:11:31
 *
 *  @brief  回调开始重连
 */
- (void)callBackStartReConnect{
    
    KCTError * error = [KCTError errorWithCode:ErrorCode_NoError andDescription:@"开始重连,没有错误"];
    
    if (self.tcpDeleagte && [self.tcpDeleagte respondsToSelector:@selector(didConnectionStatusChanged:error:)]) {
        [self.tcpDeleagte didConnectionStatusChanged:KCTConnectionStatus_StartReConnect error:error];
    }
    
    [self callBackConnectionStateToSDK:KCTConnectionStatus_StartReConnect error:error];
    
}


/*!
 *  @author barry, 15-11-12 15:11:31
 *
 *  @brief  回调正在重连
 */
- (void)callBackReConnecting{

    KCTError * error = [KCTError errorWithCode:ErrorCode_NoError andDescription:@"正在重连,没有错误"];
    
    if (self.tcpDeleagte && [self.tcpDeleagte respondsToSelector:@selector(didConnectionStatusChanged:error:)]) {
        [self.tcpDeleagte didConnectionStatusChanged:KCTConnectionStatus_ReConnecting error:error];
    }
    
     [self callBackConnectionStateToSDK:KCTConnectionStatus_ReConnecting error:error];
}



/*!
 *  @author barry, 15-11-12 15:11:06
 *
 *  @brief  回调重连接成功
 *
 *  @param error <#error description#>
 */
- (void)callBackReConnectSuccess{
    
    KCTError * error = [KCTError errorWithCode:ErrorCode_NoError andDescription:@"重连成功,没有错误"];
    
    if (self.tcpDeleagte && [self.tcpDeleagte respondsToSelector:@selector(didConnectionStatusChanged:error:)]) {
        [self.tcpDeleagte didConnectionStatusChanged:KCTConnectionStatus_ReConnectSuccess error:error];
    }
    
     [self callBackConnectionStateToSDK:KCTConnectionStatus_ReConnectSuccess error:error];
}

/*!
 *  @author barry, 15-11-12 15:11:43
 *
 *  @brief  回调重连失败
 */
- (void)callBackReConnectFail{
    
    KCTError * error;
    if ([UCSTCPManager instance].isClientOnline == NO) {
       error = [KCTError errorWithCode:ErrorCode_InvalidToken andDescription:@"自动重连失败,可能是token失效了,请重新登陆"];
    }else{
       error = [KCTError errorWithCode:ErrorCode_ReConnectToServerFail andDescription:@"自动重连失败,稍后会继续重连"];
    }
    
    if (self.tcpDeleagte && [self.tcpDeleagte respondsToSelector:@selector(didConnectionStatusChanged:error:)]) {
        [self.tcpDeleagte didConnectionStatusChanged:KCTConnectionStatus_ReConnectFail error:error];
        
        
        /*!
         *  @author barry, 15-11-12 16:11:15
         *
         *  @brief  将这个方法移到这里，龙思请注意
         */
        [[UCSSendMessageQueue instance] reSendAllMessageWithFaileError:error];
    }
    
     [self callBackConnectionStateToSDK:KCTConnectionStatus_ReConnectFail error:error];
}



/*!
 *  @author barry, 15-11-12 15:11:36
 *
 *  @brief  回调被T线
 *
 *  @param error <#error description#>
 */
- (void)callBackBeClickedWithError:(KCTError *)error{
    
    [self.completionBlocks removeAllObjects];
    
    if (self.tcpDeleagte && [self.tcpDeleagte respondsToSelector:@selector(didConnectionStatusChanged:error:)]) {
        [self.tcpDeleagte didConnectionStatusChanged:KCTConnectionStatus_BeClicked error:error];
    }
    
    [self callBackConnectionStateToSDK:KCTConnectionStatus_BeClicked error:error];
    
}


/*!
 *  @author barry, 15-11-17 16:11:43
 *
 *  @brief  反向调用IMClient和VoipClient中的方法
 *
 *  @param status 状态
 *  @param error  错误
 */
- (void)callBackConnectionStateToSDK:(KCTConnectionStatus) status  error:(KCTError *) error{
    
    NSString * statusStr  = [NSString stringWithFormat:@"%d", status];
    
    if (self.imClient && [self.imClient respondsToSelector:@selector(didConnectionStatusChanged:error:)]) {
        [self.imClient performSelector:@selector(didConnectionStatusChanged:error:) withObject:statusStr withObject:error];
    }
    
    
    if (self.voipClient && [self.voipClient respondsToSelector:@selector(didConnectionStatusChanged:error:)]) {
        [self.voipClient performSelector:@selector(didConnectionStatusChanged:error:) withObject:statusStr withObject:error];
    }
    
    
}

- (BOOL)isSocketIPV6
{
    return [[UCSTCPManager instance] isSocketIPV6];
}


@end
