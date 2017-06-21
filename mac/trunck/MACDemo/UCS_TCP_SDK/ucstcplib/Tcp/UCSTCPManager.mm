
//  UCSTCPManager.m
//  Tcplib
//
//  Created by FredZhang on 15-4-2.
//  Copyright (c) 2016年 KCTMAC. All rights reserved.
//

#import "UCSTCPManager.h"
#import "KCTTcpClient.h"
#import "TcpCustomLogger.h"
#import "UCSTCPProxyManager.h"


#import <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

#import "package.h"
#import "UCSSendMessageQueue.h"

#import "UCSTCPConst.h"
#import "UCSTCPTransParentConvert.h"



//add by wenqinglin
#define SOCKET_TCP_Version_TAG    1000
#define SOCKET_TCP_Login_TAG      1001
#define SOCKET_TCP_header_TAG     1002

#define SOCKET_TCP_BODY_TAG 2
#define SOCKET_TCP_HEAD_TAG 1
#define SOCKET_TCP_LOGIN    3

#define MAX_TCP_TIME 3          // 超过这个次数，连接下一个
#define MAX_HTTP_TIME 3         // 超过这个次数，获取接入列表
#define MAX_HEARTBEAT_TIME 3    // 超过这个次数，重新连接
#define MAX_DELAY_TIME 30       // 超过这个时间，就不要连接
#define HEARTBEAT_DELAY_TIME  10 // 心跳退火时间

#define HEARTBEAT_TIMER_DELAY  30  ///心跳定时器间隔
#define HEARTBEAT_TIMEOUT      40  ///最大心跳超时
#define ID_MSGBOX_PWD_CHANGED       1000
#define MAX_CACHE_FRAME             100




@interface UCSTCPManager ()
{
    dispatch_queue_t _ipQueue;  //获取ip的queue
    NSTimeInterval _reConnectInterval;
}

@property int    heartbeatResendTimes;       /// 心跳发送次数

@property (nonatomic, copy) NSString *currentServer; /// 当前已连接的 ip

@property NSMutableArray *sendFrameQueue;    /// 消息发送队列

@property (nonatomic, strong) KCTError * error;  //记录socket将断开时候的错误

@end

static UCSTCPManager *g_instance = nil;

static void set_tcp_nodelay(GCDAsyncSocket *a)
{
    CFSocketNativeHandle raw = -1;
    BOOL isIpv4 = [a isIPv4];
    if (isIpv4) {
        raw = [a socket4FD];

    } else {
        raw = [a socket6FD];
    }
    
    if (raw <= 0) {
        return;
    }
    int enable = 1;
    setsockopt(raw, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
}


@implementation UCSTCPManager{
    
    NSMutableData * _recvData;    //用来存储socket接收到的数据
    NSLock * _connectedLock;
    NSTimer * _singalTimer;
}


static id _instace;

+(instancetype)instance
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

+(id)copyWithZone:(struct _NSZone *)zone
{
    return _instace;
}


- (id)init
{
    if (self = [super init]) {
        
        _ipQueue = dispatch_queue_create("ucsIpQueue", NULL);
        
        _recvData = [[NSMutableData alloc]init];
        
        _sendFrameQueue = [[NSMutableArray alloc] init];
        _connectedLock = [[NSLock alloc] init];
        _offlinePushClosed = NO;   //默认推送不关闭
        
        //监听网络变化
        //[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(reachabilityChanged:) name:UCSTCPNetworkingReachabilityDidChangeNotification object:nil];

    }
    return self;
}


- (BOOL)ImSupport{
    return (_imClient == nil)? NO : YES;
}

/**
 *  socket懒加载
 *  @return socket
 */
- (GCDAsyncSocket *)socket{
    if (_socket == nil) {
        dispatch_queue_t mainQueue = dispatch_get_main_queue();
        _socket = [[GCDAsyncSocket alloc] initWithDelegate:self delegateQueue:mainQueue];
    }
    
    return _socket;
}


/*!
 *  @brief  设置  离线推送是否关闭的标识
 *
 *  @param offlinePushClosed yes关闭，no 不关闭
 */
-(void)setOfflinePushClosed:(BOOL)offlinePushClosed{
    
    _offlinePushClosed = YES;
}


- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    _socket = nil;
    
}


/*!
 *  @brief  登陆成功，开始发送心跳
 */
-(void)onConnectSuccessfull
{
    //[[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"UCSTCPManager--onConnectSuccessfull"];
    [UCSTCPRealConnecter sharedInstance].realConnected = YES;
    
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"登陆或者重连成功"];
    
    //设置登陆成功标志
    [UCSTCPManager instance].isClientOnline = YES;
    
    //设置心跳
    [self setHeartBeatTimer];
    
}

- (void)sendLogoutPack{
    //sendHeaderData
    NSMutableDictionary *sendDic = [[NSMutableDictionary alloc] init];
    
    [sendDic setObject:[NSNumber numberWithInt:0] forKey:@"type"];
    [sendDic setObject:[NSNumber numberWithInt:4] forKey:@"op"];
    [sendDic setObject:[NSNumber numberWithInt:0] forKey:@"enc"];
    [sendDic setObject:[NSNumber numberWithInt:0] forKey:@"sn"];
    [sendDic setObject:[NSNumber numberWithInt:0] forKey:@"tuid"];
    [sendDic setObject:[NSNumber numberWithInt:0] forKey:@"fuid"];
    [sendDic setObject:[NSNumber numberWithInt:0] forKey:@"atype"];
    
    NuBSON *bson = [NuBSON bsonWithDictionary:sendDic];
    NSData *sendHeaderData = [bson dataRepresentation];
    
    
    //sendheadS
    packTagHead sendPHead;
    memset(&sendPHead, 0, sizeof(sendPHead));
    sendPHead.headerLength = htons(sendHeaderData.length);
    sendPHead.packageLength = 0;
    NSData *sendheadS = [NSData dataWithBytes:&sendPHead length:sizeof(sendPHead)];
    
    
    //sendTotal
    NSMutableData *sendTotal = [[NSMutableData alloc]init];
    [sendTotal appendData:sendheadS];
    [sendTotal appendData:sendHeaderData];
    
    [self sendData:sendTotal];
    
}


- (void)disconnectServer:(BOOL) isLoginOut{
    //[[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"UCSTCPManager--disconnectServer"];
    [UCSTCPRealConnecter sharedInstance].realConnected = NO;
    
    if (isLoginOut) {
        //回调退出
        [self connectionDidDisConnectForSignOut];
    }

    _offlinePushClosed = NO;
    self.isClientOnline = NO;
    
    [self cancelHeartBeatTimer];

    [self cleanSocketDelegateAndDisconnectSocket];
    //解锁
    [self cleanLock];
    
    //[[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:[NSString stringWithFormat:@"isLoginOut %d", isLoginOut]];
    
    
    /**
     @author WLS, 15-12-10 12:12:06
     
     主动断开登陆或者被提线之后，voip移除所有队列里面的包，重新登陆成功不在发送
     */
    [[UCSSendMessageQueue instance] removeAllMessage];
}


- (BOOL)isConnected{
    return [_socket isConnected];
}



#pragma mark 登录

/*!
 *  @brief   tcp发起连接
 */
- (void)connect2TCP{
    
    self.socket;
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"开始连接"];
    self.isLogining = YES;
    //判断有没有网络，没有网络不连接
    //判断是不是第一次连接，第一次连接回调登陆失败
    KCTNetworkStatus netmode = KCTReachableViaWWAN;
    
    if (netmode == KCTReachableViaUnknown) {
        
        if (self.isClientOnline == NO) {
            KCTError * error = [KCTError errorWithCode:ErrorCode_NetworkIsNotConnected andDescription:@"网络未连接"];
            [[NSNotificationCenter defaultCenter] postNotificationName:KTcpHandleFailWhenLogin object:error];
        }
        
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"没网,取消连接"];
        
        return ;
    }
    
    if ([_connectedLock tryLock] == NO) {
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"Reconneting, ignore"];
        return ; // 兼容旧方法，防止多次登录
    }

    //清除socket的代理并断开socket连接 by WLS 20151027
    [self cleanSocketDelegateAndDisconnectSocket];
    [_socket setDelegate:self];
    
    [_recvData setLength:0];
    
    [self getTcpLocationIp];
    
    NSError *error = nil;
    [_socket connectToHost:self.ip onPort:[self.port intValue] withTimeout:10 error:&error];
    if (error) {
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"握手失败,取消连接"];
        
        //登陆标志为NO,说明是登陆，不是重连，这时候握手失败，我们直接返回登陆失败
        if (self.isClientOnline == NO) {
            KCTError * error = [KCTError errorWithCode:ErrorCode_ConnectToServerFail andDescription:@"连接服务器失败"];
            [[NSNotificationCenter defaultCenter] postNotificationName:KTcpHandleFailWhenLogin object:error];
        }

        [self cleanLock];
    }

}

/**
 @author WLS, 16-05-13 16:05:23
 
 获取本地Ip地址
 */
- (void)getTcpLocationIp{
    
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    
    NSString * tcpIp = [defaults valueForKey:@"tcpAddress"];
    NSString * tcpPort = [defaults valueForKey:@"tcpPort"];
    
    if (![tcpIp isKindOfClass:[NSNull class]] &&
        ![tcpIp isEqualToString:@""] &&
        tcpIp) {
        self.ip = tcpIp;
        self.port = tcpPort;
    }
    
}

/*!
 *  @brief  发送数据
 */
-(void)sendData:(NSData *)sendData{
    dispatch_async(dispatch_get_main_queue(), ^(void){
        [_socket writeData:sendData withTimeout:-1 tag:0];
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:[NSString stringWithFormat:@"---send data leng :%d",[sendData length]]];
    });
}



#pragma mark  handleSocketData 

-(void)handleReceiveSocketData:(NSData *)reData
{
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:[NSString stringWithFormat:@"解释包的数据 长度：%d",reData.length]];
    
    packTagHead pHead;
    memset(&pHead, 0, sizeof(pHead));
    [reData getBytes:(void *)&pHead length:sizeof(pHead)];
    pHead.headerLength = CFSwapInt16(pHead.headerLength);
    pHead.packageLength = CFSwapInt16(pHead.packageLength);
    NSData *headData = [reData subdataWithRange:NSMakeRange(4,pHead.headerLength)];
    if (headData) {
        NuBSON *headBson = [NuBSON bsonWithData:headData];
        if (headBson) {
            //有bson数据，说明是KC 的数据
            NSData *packData = [reData subdataWithRange:NSMakeRange(4+pHead.headerLength,pHead.packageLength)];
            NSNumber *typeN = [headBson objectForKey:@"type"];
            NSNumber *opN = [headBson objectForKey:@"op"];
            
            NSDictionary * packDict = [NSJSONSerialization JSONObjectWithData:packData options:NSJSONReadingMutableContainers error:nil];
            NSNumber *nResult = [packDict objectForKey:@"result"];
            
            if (typeN != nil && [typeN intValue] == 0x00) {
                // 客户端与服务端内部消息
                if ([opN intValue] == 5) {
                    //连接建立
                    NSDictionary * packDict = [NSJSONSerialization JSONObjectWithData:packData options:NSJSONReadingMutableContainers error:nil];
                    NSNumber *randcode = [packDict objectForKey:@"randcode"];
                    if (randcode) {
                        [self sendLoginPack:randcode];
                    } else {
                        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail: @"获取 randcode 失败"];
                    }
                } else if ([opN intValue] == 1) {
                    // 登录
                    if (nResult != nil && [nResult intValue] == 0) {
                        
                        [[NSNotificationCenter defaultCenter] postNotificationName:kNotiResponse_Login_1010 object:nil];
                        
                        //不处于链接过程中 by WLS 20151026
                        self.beingConnected = NO;
                        self.isLogining = NO;
                        [self sendHeartbeatData];
                    } else {
                        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail: @"登录失败"];
                    }
                } else if ([opN intValue] == 6) {
                    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"别处登陆,被T下线"];
                    [self handleBeClickedData];
                }
            } else if ([typeN intValue] == 0xB) {
                 //实时消息
            } else if ([typeN intValue] == 0xC) {
                  //存储转发消息
            } else if ([typeN intValue] == 0x05) {
                // 状态查询
            } else if ([typeN intValue] == 0xE) {
                // 服务器推送业务
            } else if ([typeN intValue] == 0xD) {
                //广播消息
            } else if ([typeN intValue] == 0xA) {
                
                [self transDataToUgo:reData];
            } else {
                
            }
            
        } else {
            IMTCPFrameHead head;
            
            memset(&head, 0, sizeof(head)); //结构体清零
            [reData getBytes:(void *)&head length:sizeof(head)];  ///从 _recvData中拷贝sizeof(head)个字节到&head中
            head.cmd = ntohl(head.cmd);
            
            
            //被T线判断
            int msgSeq;
            NSRange arange;
            arange.length = sizeof(int);
            arange.location = sizeof(IMTCPFrameHead);
            [reData getBytes:&msgSeq range:arange];
            msgSeq = ntohl(msgSeq);
            
            //IM
            if (self.imClient && [self.imClient respondsToSelector:@selector(didReceiveData:withError:)]) {
                
                [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"收到im数据"];
                KCTError *error = [KCTError errorWithCode:ErrorCode_NoError andDescription:@"没有错误"];
                [self.imClient performSelector:@selector(didReceiveData:withError:) withObject:reData withObject:error];
            }
        }
    }
}


- (void)transDataToUgo:(NSData *)data
{
    if (self.voipClient && [self.voipClient respondsToSelector:@selector(didVoipReceiveMessage:withData:)]){
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"收到voip数据"];
        KCTError *error = [KCTError errorWithCode:ErrorCode_NoError andDescription:@"没有错误"];
        [self.voipClient performSelector:@selector(didVoipReceiveMessage:withData:) withObject:error withObject:data];
    }
}


/*!
 *  @brief  处理收到的socket数据，这个数据是包体的数据
 */
-(void)handleReceiveSocketData2:(NSData *)reData
{
    IMTCPFrameHead head;
    memset(&head, 0, sizeof(head)); //结构体清零
    [reData getBytes:(void *)&head length:sizeof(head)];  ///从 _recvData中拷贝sizeof(head)个字节到&head中
    head.cmd = ntohl(head.cmd);

    
    //被T线判断
    int msgSeq;
    NSRange arange;
    arange.length = sizeof(int);
    arange.location = sizeof(IMTCPFrameHead);
    [reData getBytes:&msgSeq range:arange];
    msgSeq = ntohl(msgSeq);
    
    if (msgSeq == 8192){
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"别处登陆,被T下线"];
        [self handleBeClickedData];
        return;
    }
    
    
    
    //登陆结果返回判断
    if (head.cmd==ProxyProtocol::RESP_AUTH) {
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"登陆结果返回"];
        [[NSNotificationCenter defaultCenter] postNotificationName:kNotiResponse_Login_1010 object:reData];
        
        //不处于链接过程中 by WLS 20151026
        self.beingConnected = NO;
        return;
    }
    
    
    //重连结果返回判断
    if (head.cmd==ProxyProtocol::RESP_REAUTH) {
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"重连结果返回"];
        [[NSNotificationCenter defaultCenter] postNotificationName:kNotiResponse_reLogin_1020 object:reData];
        
        //不处于链接过程中 by WLS 20151026
        self.beingConnected = NO;
        return;
    }
    
    
    //voip和企业通信判断
    if ( (head.cmd == 2100)|| (head.cmd == 3000) ){
        if (self.voipClient && [self.voipClient respondsToSelector:@selector(didVoipReceiveMessage:withData:)]){
            [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"收到voip数据"];
            KCTError *error = [KCTError errorWithCode:ErrorCode_NoError andDescription:@"没有错误"];
            [self.voipClient performSelector:@selector(didVoipReceiveMessage:withData:) withObject:error withObject:reData];
        }
        return;
    }
    
    
    //公共透传
    if (head.cmd == 4000) {
        [UCSTCPTransParentConvert handleTransReponse:reData];
        return;
    }
    
    
    //IM
    if (self.imClient && [self.imClient respondsToSelector:@selector(didReceiveData:withError:)]) {
        
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"收到im数据"];
        KCTError *error = [KCTError errorWithCode:ErrorCode_NoError andDescription:@"没有错误"];
        [self.imClient performSelector:@selector(didReceiveData:withError:) withObject:reData withObject:error];
    }
    
}

- (void)sendLoginPack:(NSNumber *)randcode {
    
    //sendHeaderData
    NSMutableDictionary *sendDic = [[NSMutableDictionary alloc] init];
    
    [sendDic setObject:[NSNumber numberWithInt:0] forKey:@"type"];
    [sendDic setObject:[NSNumber numberWithInt:1] forKey:@"op"];
    [sendDic setObject:[NSNumber numberWithInt:0] forKey:@"enc"];
    [sendDic setObject:[NSNumber numberWithInt:0] forKey:@"sn"];
    [sendDic setObject:[NSNumber numberWithInt:0] forKey:@"tuid"];
    [sendDic setObject:[NSNumber numberWithInt:0] forKey:@"fuid"];
    [sendDic setObject:[NSNumber numberWithInt:0] forKey:@"atype"];
    
    NuBSON *bson = [NuBSON bsonWithDictionary:sendDic];
    NSData *sendHeaderData = [bson dataRepresentation];
    
    //Byte hbb[79] = {0x4f,0x00,0x00,0x00,0x10,0x73,0x6e,0x00,0x00,0x00,0x00,0x00,0x10,0x74,0x79,0x70,0x65,0x00,0x00,0x00,0x00,0x00,0x10,0x6f,0x70,0x00,0x01,0x00,0x00,0x00,0x10,0x61,0x74,0x79,0x70,0x65,0x00,0x00,0x00,0x00,0x00,0x12,0x74,0x75,0x69,0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0x66,0x75,0x69,0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x65,0x6e,0x63,0x00,0x00,0x00,0x00,0x00,0x00};
    //NSData *sendHeaderData = [NSData dataWithBytes:hbb length:79];
    
    NSString *im_ssid = [[NSUserDefaults standardUserDefaults] objectForKey:@"KC_im_ssid"];
    if (im_ssid) {
        //sendPackData
        NSMutableDictionary *sendPackDic = [[NSMutableDictionary alloc] init];
        //[sendPackDic setObject:@"0001%g3qySU9rOu+a9kwEiwSoOkqTPGsU+Pyjenz/2k9ACSGF7kOXkmGvsrw2Aq7C5/3UvSL+J6JomY7gZhkv8srwijM70RnAUyzfKooGdZVEI1o=" forKey:@"im_ssid"];
        [sendPackDic setObject:im_ssid forKey:@"im_ssid"];
        [sendPackDic setObject:@"k_v2.18.0.0" forKey:@"version"];
        [sendPackDic setObject:[NSNumber numberWithInt:1] forKey:@"netmode"];
        [sendPackDic setObject:randcode forKey:@"randcode"];
        NSData *sendPackData = [NSJSONSerialization dataWithJSONObject:sendPackDic options:NSJSONWritingPrettyPrinted error:nil];
        
        
        //sendheadS
        packTagHead sendPHead;
        memset(&sendPHead, 0, sizeof(sendPHead));
        sendPHead.headerLength = htons(sendHeaderData.length);
        sendPHead.packageLength = htons(sendPackData.length);
        NSData *sendheadS = [NSData dataWithBytes:&sendPHead length:sizeof(sendPHead)];
        
        
        //sendTotal
        NSMutableData *sendTotal = [[NSMutableData alloc]init];
        [sendTotal appendData:sendheadS];
        [sendTotal appendData:sendHeaderData];
        [sendTotal appendData:sendPackData];
        
        [self sendData:sendTotal];
    } else {
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"im_ssid为空，登录失败"];
        return;
    }
    
}


#pragma mark - UCSTcpAsyncSocketDelegate
- (NSTimeInterval)onSocket:(GCDAsyncSocket *)sock
  shouldTimeoutReadWithTag:(long)tag
                   elapsed:(NSTimeInterval)elapsed
                 bytesDone:(NSUInteger)length{
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"读取数据超时"];
    return  -1;
}

- (NSTimeInterval)onSocket:(GCDAsyncSocket *)sock
 shouldTimeoutWriteWithTag:(long)tag
                   elapsed:(NSTimeInterval)elapsed
                 bytesDone:(NSUInteger)length{
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"写入数据超时"];
    return -1;
}


- (void)onSocket:(GCDAsyncSocket *)sock willDisconnectWithError:(NSError *)err
{
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"连接将断开"];

    //如果ip不存在，说明断开之前的连接是正常的，这次断开是异常断开，回调一次  即将异常断开
    KCTNetworkStatus netmode = KCTReachableViaWWAN;
    if (self.ip == nil) {
        if (netmode == KCTReachableViaUnknown) {
            [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"网络未连接"];
            self.error = [KCTError errorWithCode:ErrorCode_NetworkIsNotConnected andDescription:@"网络未连接"];
        }else{
            [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"未知错误"];
            self.error = [KCTError errorWithCode:ErrorCode_UnKnown andDescription:@"未知错误"];
        }
        [self ConnectionWillDisConnectWithError:self.error];
    }
    
}


- (void)socketDidDisconnect:(GCDAsyncSocket *)sock withError:(nullable NSError *)err{
    
    [UCSTCPRealConnecter sharedInstance].realConnected = NO;
    
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"连接已经断开"];

    //解锁
    [self cleanLock];
    
    if (self.isClientOnline == NO) {
        
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"连接已经断开,但当前不是登陆状态"];
       
        //发起连接
        [self connect];
        
    }else {
        
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"连接断开了,当前是登陆状态,马上回调错误，并开始重连"];
        
        
        //登陆状态下，如果ip列表最后一个ip第二次尝试连接还是失败,那么就回调一次  重连失败
        if ([UCSTCPProxyManager sharedInstance].isEnd) {
            //回调重连失败
            [[NSNotificationCenter defaultCenter] postNotificationName:kReconnectFailNotification object:nil];
        }
        
        //如果ip不存在，说明断开之前的连接是正常的，这次断开是异常断开，回调一次  已经异常断开
        if (self.ip == nil) {
            [self connectionDidDisConnectWithError:self.error];
        }
        
        //error事件断开,马上重连
        [self reConnect];
    }

}
- (void)onSocket2:(GCDAsyncSocket *)sock didConnectToHost:(NSString *)host port:(UInt16)port{
    
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"--tcp握手成功"];
    
    //握手成功，更新ip优先级
    [[UCSTCPProxyManager sharedInstance] exchangeProxy];
    
    //握手成功，清空ip,用于后面是否异常断开判断
    self.ip = nil;
    
    self.beingConnected = YES;
    
    //握手成功后，开始发送数据登陆
    //不在线，发送登陆数据；在线，发送重连数据
    [self sendData:(self.isClientOnline == NO)? self.loginData:self.reLoginData];
    
    //先读前6个字节，判断数据包结构
    set_tcp_nodelay(sock);
    [_socket readDataToLength:sizeof(socketTagHead) withTimeout:10  tag:SOCKET_TCP_HEAD_TAG];
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:[NSString stringWithFormat:@"rec the first pack leng :%d",sizeof(socketTagHead)]];
}



- (void)socket:(GCDAsyncSocket *)sock didConnectToHost:(NSString *)host port:(UInt16)port {
    
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"--tcp握手成功"];
    
    //握手成功，更新ip优先级
    [[UCSTCPProxyManager sharedInstance] exchangeProxy];
    
    //握手成功，清空ip,用于后面是否异常断开判断
    self.ip = nil;
    
    self.beingConnected = YES;
    self.isLogining = YES;
    //握手成功后，开始发送数据登陆
    //不在线，发送登陆数据；在线，发送重连数据
    [self sendData:(self.isClientOnline == NO)? self.loginData:self.reLoginData];
    
    //先读前6个字节，判断数据包结构
    set_tcp_nodelay(sock);
    
    [_socket readDataToLength:sizeof(packTagHead) withTimeout:10  tag:SOCKET_TCP_HEAD_TAG];
    
}

- (BOOL)isSocketIPV6
{
    BOOL isIpv6 = [_socket isIPv6];
    return isIpv6;
}

- (void)socket:(GCDAsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag
{
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:[NSString stringWithFormat:@"tcp收到数据 长度是：%d",data.length]];
    
    _heartbeatResendTimes = 0;
    
    if (tag == SOCKET_TCP_HEAD_TAG) {
        [_recvData appendData:data];
        if (data.length == sizeof(packTagHead)) {
            packTagHead headSturctd;
            memset(&headSturctd, 0, sizeof(packTagHead));
            [data getBytes:&headSturctd length:sizeof(packTagHead)];
            NSInteger totalLength = ntohs(headSturctd.packageLength) + ntohs(headSturctd.headerLength);
            if (totalLength > 0) {
                [_socket readDataToLength:totalLength withTimeout:-1 tag:SOCKET_TCP_BODY_TAG];
            } else {
                [_recvData setLength:0];
                [_socket readDataToLength:sizeof(packTagHead) withTimeout:-1 tag:SOCKET_TCP_HEAD_TAG];
            }
        } else {
            IMTCPFrameHead head;
            memset(&head, 0, sizeof(head)); //对结构体清零
            [_recvData getBytes:(void *)&head length:sizeof(head)]; //从 _recvData中拷贝sizeof(head)个字节到&head中
            head.packageLength = ntohl(head.packageLength);  //ntohl()是将一个无符号长整形数从网络字节顺序转换为主机字节顺序。
            head.headerLength = ntohs(head.headerLength);
            int leftLength = head.packageLength - head.headerLength;   // 获取包体的总长度
            
            if (leftLength>0) {
                [_socket readDataToLength:leftLength withTimeout:-1 tag:SOCKET_TCP_BODY_TAG];
            }else {
                // 心跳，忽略
                [_recvData setLength:0];
                [_socket readDataToLength:sizeof(socketTagHead) withTimeout:-1 tag:SOCKET_TCP_HEAD_TAG];
            }
        }
    } else if (tag == SOCKET_TCP_BODY_TAG) {
        [_recvData appendData:data];
        
        NSData *dataS = [_recvData copy];
        [_recvData setLength:0];
        [self handleReceiveSocketData:dataS];
        [_socket readDataToLength:sizeof(packTagHead) withTimeout:-1 tag:SOCKET_TCP_HEAD_TAG];
    }
}


- (void)onSocket2:(GCDAsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag
{
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"tcp收到数据"];
    NSString *respone = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    _heartbeatResendTimes = 0;
    
    if ( tag == SOCKET_TCP_HEAD_TAG ) {
        
        [_recvData appendData:data];//追加数据
        
        //如果是对某个数据包 包头 的第一次读取
        if (_recvData.length == sizeof(socketTagHead) && data.length == sizeof(socketTagHead)) {
    
            socketTagHead tagHead;
            memset(&tagHead, 0, sizeof(tagHead));
            [_recvData getBytes:(void *)&tagHead length:sizeof(tagHead)];
            tagHead.headerLength = ntohs(tagHead.headerLength);
            NSInteger headerLeftLength = tagHead.headerLength - sizeof(socketTagHead);
            //读包头剩下的字节
            [_socket readDataToLength:headerLeftLength withTimeout:10 tag:SOCKET_TCP_HEAD_TAG];
        }else{
            
            IMTCPFrameHead head;
            memset(&head, 0, sizeof(head)); //对结构体清零
            [_recvData getBytes:(void *)&head length:sizeof(head)]; //从 _recvData中拷贝sizeof(head)个字节到&head中
            head.packageLength = ntohl(head.packageLength);  //ntohl()是将一个无符号长整形数从网络字节顺序转换为主机字节顺序。
            head.headerLength = ntohs(head.headerLength);
            int leftLength = head.packageLength - head.headerLength;   // 获取包体的总长度

            if (leftLength>0) {
                [_socket readDataToLength:leftLength withTimeout:-1 tag:SOCKET_TCP_BODY_TAG];
            }else {
                // 心跳，忽略
                [_recvData setLength:0];
                [_socket readDataToLength:sizeof(socketTagHead) withTimeout:-1 tag:SOCKET_TCP_HEAD_TAG];
            }

        }
        
        
    }else if ( tag == SOCKET_TCP_BODY_TAG ) {
        
        [_recvData appendData:data];
        
        NSData *data = [_recvData copy];
        [self handleReceiveSocketData:data];
        
        [_recvData setLength:0];
        [_socket readDataToLength:sizeof(socketTagHead) withTimeout:-1 tag:SOCKET_TCP_HEAD_TAG];
        
    }else{};
    
    
}



- (void)socket:(GCDAsyncSocket *)sock didWriteDataWithTag:(long)tag{
    
    //sokect  完成的回调
    if (_offlinePushClosed == YES && tag == kSocket_tcp_offLinePush_tag)
    {
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"已经写入注销离线推送的数据，并且_offlinePushClosed = yes, 完成注销"];
        
        //广播tcp主动断开
        [[NSNotificationCenter defaultCenter] postNotificationName:UCSTCPDisconnectTcpConnectionActiveNotification object:nil];
        
        [[UCSTCPManager instance] disconnectServer:YES];
        _offlinePushClosed = NO;
    }
    
}


#pragma mark 心跳包

- (void)setHeartBeatTimer{
    //取消心跳
    [self cancelHeartBeatTimer];
    //开始心跳
    _heartbeatTimer = [NSTimer scheduledTimerWithTimeInterval:HEARTBEAT_TIMER_DELAY target:self selector:@selector(heartbeat) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:_heartbeatTimer forMode:NSRunLoopCommonModes];
    _idleSeconds = _heartbeatResendTimes = 0;
}

/*!
 *  @brief  取消心跳
 */
- (void)cancelHeartBeatTimer{
    
    if (_heartbeatTimer) {
        [_heartbeatTimer invalidate];
        _heartbeatTimer = nil;
    }
}

- (void)setKeepAlive{
    
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@" "];
    KCTNetworkStatus netmode = KCTReachableViaWWAN;
    if (netmode == KCTReachableViaUnknown) {
        return;
    }
    
    if (!self.isConnected) {
        [self reConnect];
        return;
    }
    
    [self sendHeartbeatData];
    
    double delayInSeconds = 3;
    dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayInSeconds * NSEC_PER_SEC));
    dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
        
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:[NSString stringWithFormat:@"heartbeatResendTimes=%d", _heartbeatResendTimes]];
        if (_heartbeatResendTimes > 0) {
            [self reConnect];
        }
    });
}


- (void)heartbeat
{
    if (self.isClientOnline == NO) {
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"断开状态下，心跳失效"];
        return;
    }
    

    KCTNetworkStatus netmode = KCTReachableViaWWAN;
    if (netmode == KCTReachableViaUnknown) {
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"网都断了,啥都不能干"];
        return;
    }
    
    if (!self.isConnected ) {
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"连接断开了,重新连接"];
        [self reConnect];
        
    }else{
        
        if ((time(NULL) - _idleSeconds) < HEARTBEAT_TIMEOUT) {
             [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"未达到最大超时,返回"];
            return;
        }
        
        [self sendHeartbeatData];
        
        _idleSeconds = (int)time(NULL);
        _heartbeatResendTimes += 1;
        double delayInSeconds = _heartbeatResendTimes * HEARTBEAT_DELAY_TIME;
        dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayInSeconds * NSEC_PER_SEC));
        dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
            [self resendHeartbeat];
        });
        
    }
    
}

/*!
 *  @author barry, 15-11-18 10:11:29
 *
 *  @brief  检测心跳次数，次数大于规定值就说明TCP假死了，重连
 */
- (void)resendHeartbeat
{
    if (_heartbeatResendTimes <= 0) {
        return;
    } else if (_heartbeatResendTimes >= MAX_HEARTBEAT_TIME) {
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"服务器长时间未响应心跳,重连"];
        _heartbeatResendTimes = MAX_HEARTBEAT_TIME;
        [self cancelHeartBeatTimer];
        [self reConnect];
    } else {
        _idleSeconds = 0; // 清除心跳空闲
        [self heartbeat];
    }
    
}

/**
 *  @brief 发送心跳数据
 */
- (void)sendHeartbeatData{
    printf("------发送心跳数据 islogin: %d\n",self.isLogining);
    if (!self.isLogining) {
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"发送心跳数据"];
        Byte bData[4] = {0x00,0x00,0x00,0x00};
        NSData * data  = [NSData dataWithBytes:bData length:4];
        [[UCSTCPManager instance] sendData:data];
    }
    
}


#pragma mark - TCP Notification
- (void)reachabilityChanged:(NSNotification *)note
{
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"网络变化了"];
    
    if (self.isClientOnline ) {
        
        [self cleanSocketDelegateAndDisconnectSocket];
        KCTNetworkStatus netmode = KCTReachableViaWWAN;
        if (netmode == KCTReachableViaUnknown) {
            [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"开始重连"];
            [self reConnect];
        }else{
            
            /**
             @author WLS, 15-12-10 16:12:14
             
             当检测到无网络的时候，断开链接，然后上抛tcp断开的错误
             */
            KCTError * error ;
            [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"网络未连接"];
            error = [KCTError errorWithCode:ErrorCode_NetworkIsNotConnected andDescription:@"网络未连接"];

            //记录错误
            self.error = error;
            
            [self ConnectionWillDisConnectWithError:error];
            
        }
        
    }
    
}


/*!
 *  @author barry, 15-10-08 15:10:45
 *
 *  @brief  tcp连接异常情况
 *
 *  @param error <#error description#>
 */
-(void)ConnectionWillDisConnectWithError:(KCTError *)error
{
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"tcp将断开回调"];
    [[NSNotificationCenter defaultCenter] postNotificationName:kConnectionWillDisConnectNotification object:error];

}


#pragma mark - 切换IP登陆
/**
 *  切换ip登陆
 */
- (void)connect{
    
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"切换ip登陆"];
    [self cleanLock];
    [self cleanSocketDelegateAndDisconnectSocket];
    KCTNetworkStatus netmode = KCTReachableViaWWAN;
    if (netmode == KCTReachableViaUnknown) {
        //切换ip登陆
        [[NSNotificationCenter defaultCenter] postNotificationName:kConnectWithNextIpNotification object:nil];
    }else {
        //直接判断为登陆失败
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"没有网络,登陆失败"];
    }
}


#pragma mark - 重连\取消重连
- (void)cancelReConnect{
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"取消重连"];
    
    [self cleanLock];
    //如果用户使用错误的key重连，会导致包解析错误，但是后台不会主动断开socket，这时候如果发送消息之类的，还是会收到回执，这时候容易出现错误。断开连接，以免继续收到回执。
    [[UCSTCPManager instance] cleanSocketDelegateAndDisconnectSocket];
    self.isClientOnline = NO;
    [self cancelHeartBeatTimer];
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
}
/*!
 *  @brief  重连
 */
- (void)reConnect{
    NSTimeInterval nowInterval = [[NSDate date] timeIntervalSince1970];
    printf("-----reconnect ---%f\n",nowInterval-_reConnectInterval);
    if (nowInterval-_reConnectInterval < 2) {
        return;
    }
    _reConnectInterval = nowInterval;
    
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"重连"];
    
    [self cleanLock];
    [self cleanSocketDelegateAndDisconnectSocket];
    KCTNetworkStatus netmode = KCTReachableViaWWAN;
    if (netmode == KCTReachableViaUnknown) {
        [[NSNotificationCenter defaultCenter] postNotificationName:kReLogin_Request_Notification object:nil];
    }else {
        [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"没有网络,不重连"];
    }
}

/*!
 *  @author barry, 15-10-09 12:10:23
 *
 *  @brief  清除锁
 */
- (void)cleanLock{
    [_connectedLock tryLock];
    [_connectedLock unlock];
}

/*!
 *  @author barry, 15-10-09 12:10:02
 *
 *  @brief  清除socket的代理并断开socket连接
 */
- (void)cleanSocketDelegateAndDisconnectSocket{
    [_socket setDelegate:nil];
    [_socket disconnect];
    
    //此时不处于链接过程中 add by WLS 20151027
    self.beingConnected = NO;
}



/*!
 *  @author barry, 15-10-09 16:10:58
 *
 *  @brief  连接断开，通知voip
 *
 *  @param error 错误类
 */
- (void)connectionDidDisConnectWithError:(KCTError *) error{
    
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"tcp已经断开回调"];
    
    //回调给开发者
    if (self.tcpDelegate && [self.tcpDelegate respondsToSelector:@selector(didConnectionStatusChanged:error:)]) {
        [self.tcpDelegate didConnectionStatusChanged:KCTConnectionStatus_AbnormalDisconnection error:error];
    }
    
    //回调给sdk
    NSString * statusStr  = [NSString stringWithFormat:@"%d", KCTConnectionStatus_AbnormalDisconnection];
    
    if (self.imClient && [self.imClient respondsToSelector:@selector(didConnectionStatusChanged:error:)]) {
        [self.imClient performSelector:@selector(didConnectionStatusChanged:error:) withObject:statusStr withObject:error];
    }

    if (self.voipClient && [self.voipClient respondsToSelector:@selector(didConnectionStatusChanged:error:)]) {
        [self.voipClient performSelector:@selector(didConnectionStatusChanged:error:) withObject:statusStr withObject:error];
    }
    
}

/*!
 *  @author barry, 15-11-17 17:11:26
 *
 *  @brief  主动退出，回调sdk和开发者
 */
- (void)connectionDidDisConnectForSignOut{
    
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"主动退出,回调"];
    
    KCTError * error = [KCTError errorWithCode:ErrorCode_NoError andDescription:@"主动断开连接"];
    //回调给开发者
    if (self.tcpDelegate && [self.tcpDelegate respondsToSelector:@selector(didConnectionStatusChanged:error:)]) {
        [self.tcpDelegate didConnectionStatusChanged:KCTConnectionStatus_SignOut error:error];
    }
    
    //回调给sdk
    NSString * statusStr  = [NSString stringWithFormat:@"%d", KCTConnectionStatus_SignOut];
    
    if (self.imClient && [self.imClient respondsToSelector:@selector(didConnectionStatusChanged:error:)]) {
        [self.imClient performSelector:@selector(didConnectionStatusChanged:error:) withObject:statusStr withObject:error];
    }
    
    if (self.voipClient && [self.voipClient respondsToSelector:@selector(didConnectionStatusChanged:error:)]) {
        [self.voipClient performSelector:@selector(didConnectionStatusChanged:error:) withObject:statusStr withObject:error];
    }
}



/*!
 *  @author barry, 16-01-13 11:01:16
 *
 *  @brief 被T线解析
 */
- (void)handleBeClickedData{
    
    [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"被挤下线"];
    
    // 服务器发送的通知：踢线、新消息等
    //提示开发者的上层消息
    
    //断开连接
    [self disconnectServer:YES];

    KCTError *errorCode = [KCTError errorWithCode:ErrorCode_BeClicked andDescription:@"账号在别处登陆,你被挤下线了"];
    [[NSNotificationCenter defaultCenter] postNotificationName:KTcpErrorCodeBeClicked object:errorCode];

}


@end
