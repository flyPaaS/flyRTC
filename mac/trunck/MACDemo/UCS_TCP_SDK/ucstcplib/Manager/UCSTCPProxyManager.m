//
//  UCSTCPProxyManager.m
//  Tcplib
//
//  Created by KCT on 16/2/29.
//  Copyright © 2017年 KCTMAC. All rights reserved.
//

#import "UCSTCPProxyManager.h"
#import "UCSTCPManager.h"
#import "KCTTcpClient.h"

//#define UCSTCPProxyURL      @"http://113.31.89.144:9997/v2/getproxylist?"  //测试
//#define UCSTCPProxyURL      @"http://172.16.5.22:9997/v2/getproxylist?"  //开发
#define UCSTCPProxyURL      @"http://cps.ucpaas.com:9997/v2/getproxylist?"           //获取proxy的地址前缀
#define UCS_TCP_TESTKEY      @"tcpAddress"           //环境是test环境的key

#define kReconnectDelay  10   //重连拉取proxy失败后再次进行重连的间隔时间10s
#define kSameIpDelay 3    //同一个ip再次访问延迟3s

@interface UCSTCPProxyManager ()
@property (nonatomic, copy)ProxyBlock proxy;  //回调block
@property (nonatomic, strong)NSString *currentIp; //当前作为tcp连接的ip
@property (nonatomic, assign)NSInteger index; //当前ip在ip列表中的位置
@property (nonatomic, strong)NSArray * ipArray; //当前ip的列表
@property (nonatomic, assign)BOOL isSecond; //当前currentIp是否已经被取了一次，是为yes，不是为no

@property (nonatomic, assign)BOOL isLogged; //是否登陆，判断是否需要先拉取ip列表

@property (nonatomic, assign)BOOL hasRequest; //登陆时，判断已经通过http请求了一次

@property (nonatomic, assign)BOOL isReconnectNeedDelay; //重连http请求proxy失败，需要延迟发起下一次重连

@property (nonatomic, assign)BOOL isConnectNeedDelay;  //同一个ip使用第二次，需要延迟

@end


@implementation UCSTCPProxyManager

#pragma mark  init
static id _instace;

+(instancetype)sharedInstance
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

- (instancetype)init
{
    if (self = [super init]) {
        _index = 0;
        _isSecond = NO;
        _isEnd = NO;
        _isLogged = NO;
        _hasRequest = NO;
        _isReconnectNeedDelay = NO;
        _isConnectNeedDelay = NO;
        
        /*
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(enterBackground)
                                                     name:UIApplicationDidEnterBackgroundNotification
                                                   object:nil];*/
        
        // KVO 检查是否主动退出
        [[UCSTCPManager instance] addObserver:self
                                         forKeyPath:@"isClientOnline"
                                            options:NSKeyValueObservingOptionNew|NSKeyValueObservingOptionOld
                                            context:nil];
    }
    return self;
}


- (void)dealloc{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [[UCSTCPManager instance] removeObserver:self forKeyPath:@"isClientOnline"];
}


#pragma mark Public

- (void)loadProxyWithHttp:(BOOL)isLoged iP:(ProxyBlock)proxy{
    self.proxy = proxy;
    self.isLogged = isLoged;
    
    //如果未登陆，按照未登陆流程走
    //如果已登陆，按照重连流程走
    if (isLoged == NO) {
        
        //如果是同一个ip第二次，延迟
        if (self.isSecond) {
            [self performSelector:@selector(getCurrentIpForLogin) withObject:nil afterDelay:kSameIpDelay];
        }else{
            [self getCurrentIpForLogin];
        }
        
    }else{
        
        //判断重连是否需要延迟
        if (self.isReconnectNeedDelay) {
            [self performSelector:@selector(getCurrentIpWithRelogin) withObject:nil afterDelay:kReconnectDelay];
            self.isReconnectNeedDelay = NO;
        }else if(self.isSecond){
            [self performSelector:@selector(getCurrentIpWithRelogin) withObject:nil afterDelay:kSameIpDelay];
        }else{
            [self getCurrentIpWithRelogin];
        }
        
    }
}


- (void)exchangeProxy{
    //self.index需要调整。如果isSecond,说明之前被index是被++了,所以这里需要--。
    if (self.isSecond == NO) {
        self.index --;
    }
    
    //调整proxy优先级，保存到沙盒
    NSMutableArray *mutableArray  = [self.ipArray mutableCopy];
    if (self.index >0 && mutableArray.count > self.index) {
        [mutableArray exchangeObjectAtIndex:0 withObjectAtIndex:self.index];
    }
   
    self.ipArray = [mutableArray copy];
    [self updateProxyInSanbox:self.ipArray];
    
    //还原现场
    [self revert];
}


- (void)revert{
    _currentIp = nil;
    _index = 0;
    _ipArray = nil;
    _isSecond = NO;
    _isEnd = NO;
    _hasRequest = NO;
}


#pragma mark  getter
/**
 *  获取proxy,
 *  @return 返回获取的proxy，有可能为nil
 */
- (NSArray *)ipArray{
    if (_ipArray == nil) {
        _ipArray = [self proxyFromSanbox];
    }
    return _ipArray;
}


#pragma mark private
/**
 *  获取登陆ip
 */
- (void)getCurrentIpForLogin{
    
    //未通过http请求，那么请求一次
    if (self.hasRequest == NO) {
        [self requestProxys];
        return;
    }
    
    //到这说明已经通过http请求
    
    //沙盒中取不到，说明请求失败，并且沙盒中没有ip列表
    if (self.ipArray == nil) {
        self.proxy(nil, NO, [[NSError alloc] init]);
        //还原现场，兼容旧的版本
        [self revert];
        return;
    }

    //沙盒中有，有可能是请求成功，也有可能是请求失败，从沙盒中取出得
    if (self.ipArray.count > self.index) {
        
        //取第index个ip
        self.currentIp = self.ipArray[self.index];

        //判断是否是最后一个proxy. 是的话，isEnd为yes
        if ( (self.ipArray.count == self.index + 1) && (self.isSecond == YES)) {
            self.isEnd = YES;
        }

        //回调proxy
        self.proxy(self.currentIp, self.isEnd, nil);
        
        //已经是第二次，下次index要增加
        if (self.isSecond) {
            self.index ++;
        }

        //isSecond反转
        self.isSecond = !self.isSecond;
        
    }else{
        //取完了,就返回error
        self.proxy(nil, self.isEnd, [[NSError alloc] init]);
        
        //并且还原现场
        [self revert];
    }
    
    
    
}


/**
 *  获取重连ip
 */
- (void)getCurrentIpWithRelogin{
    
    //以 self.ipArray.count > self.index  和  self.isEnd 来判断是否有下一个proxy
    
    //如果有，说明重连没有取完所有的ip
    if ( (self.ipArray.count > self.index) && (self.isEnd == NO) ) {
        
        //取第index个proxy
        self.currentIp = self.ipArray[self.index];
        
        //判断是否是最后一个proxy. 是的话，isEnd为yes
        if ((self.ipArray.count == self.index + 1) && (self.isSecond == YES)) {
            self.isEnd = YES;
        }
        
        //回调proxy
        self.proxy(self.currentIp, self.isEnd, nil);
        
        //已经是第二次，下次index要增加
        if (self.isSecond) {
            self.index ++;
        }
        
        //isSecond反转
        self.isSecond = !self.isSecond;
        
    }else{
        
        //走到这里，说明重连已经获取了一遍本地列表,需要请求新的列表
        [self requestProxys];
    }
    
    
    
}

/**
 *  从沙盒中获取proxy列表
 *  @return 获取到的proxy列表
 */
- (NSArray *)proxyFromSanbox{
    NSUserDefaults  *defaults = [NSUserDefaults standardUserDefaults];
    NSArray *proxyArray  = [defaults  objectForKey:@"proxyArray"];
    return proxyArray;
}


/**
 *  更新本地proxy列表
 *  @param proxyArray 返回的proxy列表
 */
- (void)updateProxyInSanbox:(NSArray *)proxyArray{
    NSUserDefaults  *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setObject:[proxyArray copy]forKey:@"proxyArray"];
    [defaults synchronize];
}

/*!
 *  @brief  获取tcp环境 hostName
 */
- (NSString *)ucsTcpHostName{
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    NSString * development = [defaults stringForKey:UCS_TCP_TESTKEY];
    
    //取有没有设置tcp的测试环境，有就返回设置的测试环境，没有就返回原来的值
    if (development &&  development.length > 0 ) {
        return development;
    }else{
        return nil;
    }
}

/**
 *  @brief  获取cps
 */
- (NSString *)cpsUrl{
    NSString * cpsUrl = UCSTCPProxyURL;
    //如果cps配置了，就用cps配置的
    id cpsAddress = [[NSUserDefaults standardUserDefaults] objectForKey:@"cpsAddress"];
    id cpsPort = [[NSUserDefaults standardUserDefaults] objectForKey:@"cpsPort"];
    if (cpsAddress
        && [cpsAddress isKindOfClass:[NSString class]]
        && cpsPort
        && [cpsPort isKindOfClass:[NSString class]]) {
        cpsUrl = [NSString stringWithFormat:@"http://%@:%@/v2/getproxylist?", cpsAddress, cpsPort];
    }
    
    return cpsUrl;
}


#ifdef kLocalAreaNetworkEnvironment
#define getCSListURL  @"http://192.168.0.8:8097/v3/getcslist"
#else
#define getCSListURL  @"http://59.110.10.28:8097/v3/getcslist"
#endif


- (NSMutableURLRequest *)request2{
    
    NSUserDefaults *nsDefault = [NSUserDefaults standardUserDefaults];
    NSNumber *NumUid = [nsDefault objectForKey:@"KC_uid"];
    NSString *strUid = NumUid.description;
    
    NSString *urlString;
    NSString *csUrl = [nsDefault objectForKey:@"KC_csUrl"];
    if (csUrl) {
        urlString = [NSString stringWithFormat:@"%@?",csUrl];
    } else {
        urlString = [NSString stringWithFormat:@"%@?",getCSListURL];
    }
    //printf("-----url is : %s",[urlString UTF8String]);
    
    NSURL *url  = [NSURL URLWithString:urlString];
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url cachePolicy:0 timeoutInterval:15];
    return request;
}


- (NSMutableURLRequest *)request{
    
    NSString *appid;
    NSString *userid;
    NSString *ver = @"iOS";
    
    NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
    NSString *token = [defaults objectForKey:@"TcpLoginToken"];
    
    //token存在，密文登陆;token不存在，明文登陆(旧的登陆方式)
    if (token && token.length > 0) {
        NSArray * tokenArray = [token componentsSeparatedByString:@"."];
        NSString *temp = [tokenArray objectAtIndex:0];
        
        NSData* decodeData = [[NSData alloc] initWithBase64EncodedString:temp options:0];
        NSString* decodeStr = [[NSString alloc] initWithData:decodeData encoding:NSASCIIStringEncoding];
        NSDictionary *tokenDic = [self tcpDictionaryWithJsonString:decodeStr];
        appid = [tokenDic objectForKey:@"Appid"];
        userid = [tokenDic objectForKey:@"Userid"];
        
    }else{
        appid = @"";
        NSString *clientNumber = [defaults objectForKey:@"TcpLoginClientNumber"];
        userid = (clientNumber && clientNumber.length > 0)? clientNumber : @"";
    }
    
    NSURL *url  = [NSURL URLWithString:[NSString stringWithFormat:@"%@appid=%@&userid=%@&ver=%@",[self cpsUrl], appid, userid, ver]];
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:url cachePolicy:0 timeoutInterval:15];
    return request;
}



#pragma mark http request

/**
 *  网络请求proxy
 */
- (void)requestProxys{
    
    NSMutableURLRequest *request = [self  request2];
    
    [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue mainQueue] completionHandler:^(NSURLResponse * _Nullable response, NSData * _Nullable data, NSError * _Nullable connectionError) {
        
        NSString *result = [[NSString alloc] initWithData:data  encoding:NSUTF8StringEncoding];
        NSDictionary *resultDic = [self  tcpDictionaryWithJsonString:result];
        int ret = [[resultDic  objectForKey:@"result"] intValue];
        NSDictionary *dataDic = [resultDic objectForKey:@"data"];
        NSArray *proxyArray  = [dataDic  objectForKey:@"csaddr"];
        
        //拉取成功
        if ( ret == 200 && proxyArray.count > 0) {
            
            [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"请求proxy列表成功"];
            
            //更新本地proxy列表
            [self updateProxyInSanbox:proxyArray];
            
            if (self.isLogged == NO) {
                
                //未登陆状态下，记录已经请求了
                self.hasRequest = YES;
                //回调获取IP
                [self getCurrentIpForLogin];
                
            }else{
                
                //登陆状态下,请求成功,还原现场,但是保留登陆状态。立刻回调ip,发起新一轮重连
                [self revert];
                [self getCurrentIpWithRelogin];
            }
            
            
            
        }else {
            
            [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"请求proxy列表失败"];
            
            if (self.isLogged == NO) {
                
                //未登陆状态下，记录已经请求了
                self.hasRequest = YES;
                //回调获取IP
                [self getCurrentIpForLogin];
                
            }else{
                
                //登陆状态下，请求失败，回调错误
                self.proxy(nil, YES, [[NSError alloc] init]);
                //还原现场
                [self revert];
                //下次获取需要延迟
                self.isReconnectNeedDelay = YES;
            }
            
            
            
        }
    }];
    
}
- (void)requestProxys2{
    
    NSMutableURLRequest *request = [self  request];
    
    [NSURLConnection sendAsynchronousRequest:request queue:[NSOperationQueue mainQueue] completionHandler:^(NSURLResponse * _Nullable response, NSData * _Nullable data, NSError * _Nullable connectionError) {
        
        NSString *result = [[NSString alloc] initWithData:data  encoding:NSUTF8StringEncoding];
        NSDictionary *resultDic = [self  tcpDictionaryWithJsonString:result];
        int ret = [[resultDic  objectForKey:@"ret"] intValue];
        NSArray *proxyArray  = [resultDic  objectForKey:@"proxy"];
        
        //拉取成功
        if ( ret == 0 && proxyArray.count > 0) {
            
            [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"请求proxy列表成功"];
            
            //更新本地proxy列表
            [self updateProxyInSanbox:proxyArray];
            
            if (self.isLogged == NO) {
                
                //未登陆状态下，记录已经请求了
                self.hasRequest = YES;
                //回调获取IP
                [self getCurrentIpForLogin];
                
            }else{
                
                //登陆状态下,请求成功,还原现场,但是保留登陆状态。立刻回调ip,发起新一轮重连
                [self revert];
                [self getCurrentIpWithRelogin];
            }
            


        }else {
            
            [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"请求proxy列表失败"];
            
            if (self.isLogged == NO) {
                
                //未登陆状态下，记录已经请求了
                self.hasRequest = YES;
                //回调获取IP
                [self getCurrentIpForLogin];
                
            }else{
                
                //登陆状态下，请求失败，回调错误
                self.proxy(nil, YES, [[NSError alloc] init]);
                //还原现场
                [self revert];
                //下次获取需要延迟
                self.isReconnectNeedDelay = YES;
            }
            
            

        }
    }];
    
}






#pragma mark Common

//把json转成字典
- (NSDictionary *)tcpDictionaryWithJsonString:(NSString *)jsonString {
    if (jsonString == nil) {
        return nil;
    }
    
    NSData *jsonData = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSError *err;
    NSDictionary *dic = [NSJSONSerialization JSONObjectWithData:jsonData
                                                        options:NSJSONReadingMutableContainers
                                                          error:&err];
    if(err) {
            [[TcpCustomLogger sharedInstance] log:[NSString stringWithFormat:@"%@", NSStringFromSelector(_cmd)] detail:@"json转字典失败"];
        return nil;
    }
    return dic;
}



#pragma mark enterBackGround
- (void)enterBackground{
    //进入后台，取消所有的方法
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
}

#pragma mark - Key-value Observing
//kvo检查是否退出
- (void)observeValueForKeyPath:(NSString *)keyPath
                      ofObject:(id)object
                        change:(NSDictionary *)change
                       context:(void *)context {
    if (object == [UCSTCPManager instance] && [keyPath isEqualToString:@"isClientOnline"]) {
        NSNumber *old = change[@"old"];
        NSNumber *new = change[@"new"];
        //被T或者主动退出，要取消未执行方法
        if (old.boolValue == YES  && new.boolValue == NO) {
             [NSObject cancelPreviousPerformRequestsWithTarget:self];
        }
       
    }
}


@end
