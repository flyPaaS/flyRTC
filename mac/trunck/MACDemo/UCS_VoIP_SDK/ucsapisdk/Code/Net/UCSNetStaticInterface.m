//
//  UCSNetStaticInterface.m
//  UXinClient
//
//  Created by Liam on 13-11-26.
//  Copyright (c) 2013年 UXIN CO. All rights reserved.
//

#import "UCSNetStaticInterface.h"
#import "SoftphoneManager.h"
#import "UCSTCPClientManager.h"

#define GETRTPPTIMER 900  //15分钟拉取一次rtpp add by WLS 20150819

static UCSNetStaticInterface *s_pUCSNetStaticInterface = nil;

@implementation UCSNetStaticInterface

+ (UCSNetStaticInterface *) shareInstance
{
    if (nil == s_pUCSNetStaticInterface)
    {
        s_pUCSNetStaticInterface = [[UCSNetStaticInterface alloc] init];
    }
    
    return s_pUCSNetStaticInterface;
}

- (id) init
{
    self = [super init];

    if (self)
    {
        m_pUCSHttpRequestSvrAddr = nil;
    }
    
    return self;
}

- (void) dealloc
{
    UXIN_HTTP_RELEASE(m_pUCSHttpRequestSvrAddr);
    
    if (self.getRtppTimer) {
        if ([self.getRtppTimer isValid]) {
            [self.getRtppTimer invalidate];
        }
        self.getRtppTimer = nil;
    }

    
    [super dealloc];
}

//停止定时拉取rtpp列表 WLS 20150915
- (void)stopGetRtpp{
    
    if (self.getRtppTimer) {
        if ([self.getRtppTimer isValid]) {
            [self.getRtppTimer invalidate];
            self.getRtppTimer = nil;
        }
    }
    
}


#ifdef kLocalAreaNetworkEnvironment
#define getRtpUrl   @"http://192.168.0.8:8097/v3/getrtpplist"
#else
#define getRtpUrl   @"http://59.110.10.28:8097/v3/getrtpplist"
#endif





// 服务器地址  URL_STATIC_ADDRESS--->修改为UCS_STATIC_ADDRESS  add by kucky
- (void) getTheSvrAddr
{
    NSNumber *numUid = [[NSUserDefaults standardUserDefaults] objectForKey:@"KC_uid"];
    if (numUid == nil) {
        return;
    }
    UXIN_HTTP_RELEASE(m_pUCSHttpRequestSvrAddr);
    
    //    if (![self compareDateOfGetAddr]) //初始化时判断当天是否已经获取过一次，如果当天已经获取过一次则不再获取
    //    {
    //        return;
    //    }
    
    
    //15分钟拉取一次rtpp列表 拉取一次后 开启定时器 15分钟拉取一次 add by WLS 20150819
    if (self.getRtppTimer) {
        if ([self.getRtppTimer isValid]) {
            [self.getRtppTimer invalidate];
        }
        self.getRtppTimer = nil;
    }
    if ([[UCSTCPClientManager instance]isConnected] == YES) {
        //tcp链接过程中才会去拉取rtpp add by WLS 20150909
        self.getRtppTimer = [NSTimer scheduledTimerWithTimeInterval:GETRTPPTIMER target:self selector:@selector(getTheSvrAddr) userInfo:nil repeats:NO];
        
    }
    
    NSString *ucs_static_address = nil;
    
    
    
    //获取clientNumber
    NSString * currentClientNumber = [UserDefaultManager GetclientNumber];
    
    if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"TESTMODE"])//测试
    {
        ucs_static_address = [NSString stringWithFormat:UCS_STATIC_ADDRESS_TEST,currentClientNumber];
        
        UXLogAlert(@"----rtpp----:%@",ucs_static_address);
        
    }
    else if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"DEVMODE"])//DEV 模式
    {
        ucs_static_address = [NSString stringWithFormat:UCS_STATIC_ADDRESS_DEV,currentClientNumber];
        
    }
    else //正式
    {
        NSUserDefaults *nsDefault = [NSUserDefaults standardUserDefaults];
        NSString *rtppUrl = [nsDefault objectForKey:@"KC_rtppUrl"];
        NSString *urlString;
        if (rtppUrl) {
            urlString = rtppUrl;
        } else {
            urlString = getRtpUrl;
        }
        //ucs_static_address = [NSString stringWithFormat:UCS_STATIC_ADDRESS,currentClientNumber];
        ucs_static_address = [NSString stringWithFormat:@"%@?clientnum=%@",urlString,numUid.description];
        //printf("-----url is : %s",[ucs_static_address UTF8String]);
    }
    
    
    m_pUCSHttpRequestSvrAddr = [[UCSHttpRequest alloc] initWithRequestStyle:NO isWillReturnJson:YES];
    
    [m_pUCSHttpRequestSvrAddr setURLAndDelegate:ucs_static_address withDelegate:self];
    
    [m_pUCSHttpRequestSvrAddr startAsyncRequest];
}

- (void) getTheSvrAddr3
{
    UXIN_HTTP_RELEASE(m_pUCSHttpRequestSvrAddr);
    
//    if (![self compareDateOfGetAddr]) //初始化时判断当天是否已经获取过一次，如果当天已经获取过一次则不再获取
//    {
//        return;
//    }
    
    
    //15分钟拉取一次rtpp列表 拉取一次后 开启定时器 15分钟拉取一次 add by WLS 20150819
    if (self.getRtppTimer) {
        if ([self.getRtppTimer isValid]) {
            [self.getRtppTimer invalidate];
        }
        self.getRtppTimer = nil;
    }
    if ([[UCSTCPClientManager instance]isConnected] == YES) {
        //tcp链接过程中才会去拉取rtpp add by WLS 20150909
        self.getRtppTimer = [NSTimer scheduledTimerWithTimeInterval:GETRTPPTIMER target:self selector:@selector(getTheSvrAddr) userInfo:nil repeats:NO];
        
    }
    
    NSString *ucs_static_address = nil;
    
    
    
    //获取clientNumber
    NSString * currentClientNumber = [UserDefaultManager GetclientNumber];
    
    if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"TESTMODE"])//测试
    {
        ucs_static_address = [NSString stringWithFormat:UCS_STATIC_ADDRESS_TEST,currentClientNumber];
        
        UXLogAlert(@"----rtpp----:%@",ucs_static_address);

    }
    else if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"DEVMODE"])//DEV 模式
    {
        ucs_static_address = [NSString stringWithFormat:UCS_STATIC_ADDRESS_DEV,currentClientNumber];
        
    }
    else //正式
    {
        ucs_static_address = [NSString stringWithFormat:UCS_STATIC_ADDRESS,currentClientNumber];
        
    }

    
    m_pUCSHttpRequestSvrAddr = [[UCSHttpRequest alloc] initWithRequestStyle:NO isWillReturnJson:YES];
    
    [m_pUCSHttpRequestSvrAddr setURLAndDelegate:ucs_static_address withDelegate:self];
    
    [m_pUCSHttpRequestSvrAddr startAsyncRequest];
}

// 服务器地址(备用) URL_STATIC_ADDRESS2--->修改为UCS_STATIC_ADDRESS2 add by kucky
- (void) getTheSvrAddr2
{
    UXIN_HTTP_RELEASE(m_pUCSHttpRequestSvrAddr);
    
    m_pUCSHttpRequestSvrAddr = [[UCSHttpRequest alloc] initWithRequestStyle:NO isWillReturnJson:YES];
    [m_pUCSHttpRequestSvrAddr setURLAndDelegate:UCS_STATIC_ADDRESS2 withDelegate:self];
    [m_pUCSHttpRequestSvrAddr startAsyncRequest];
}



// 比较日期(获取Addr),每天第一次
- (BOOL) compareDateOfGetAddr
{
    NSString  *lastDateStr = [UserDefaultManager GetLocalDataString:DATA_STORE_GET_STATIC_ADDR_TIME];
    
    if (nil == lastDateStr)
    {
        return YES;
    }
    
    if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"TESTMODE"])//测试
    {
        return YES;
    }
    
    if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"DEVMODE"])//DEV 模式
    {
        return YES;
    }
    
   
    NSDateFormatter *formatter = [[[NSDateFormatter alloc] init] autorelease];
    [formatter setTimeZone:[NSTimeZone localTimeZone]];
    formatter.dateFormat = @"yyyy-MM-dd";
    NSString *currDateStr = [formatter stringFromDate:[NSDate date]];
    
    if (![lastDateStr isEqualToString:currDateStr])
    {
        return YES;
    }
    
    return NO;
}

// 比较日期(获取Common),每天第一次
- (BOOL) compareDateOfGetCommon
{
    NSString  *lastDateStr = [UserDefaultManager GetLocalDataString:DATA_STORE_GET_STATIC_COMMON_TIME];
    
    if (nil == lastDateStr)
    {
        return YES;
    }
    
    NSDateFormatter *formatter = [[[NSDateFormatter alloc] init] autorelease];
    [formatter setTimeZone:[NSTimeZone localTimeZone]];
    formatter.dateFormat = @"yyyy-MM-dd";
    NSString *currDateStr = [formatter stringFromDate:[NSDate date]];
    
    if (![lastDateStr isEqualToString:currDateStr])
    {
        return YES;
    }
    
    return NO;
}

#pragma mark-
#pragma mark UCSHttpRequestDelegate methods
// 网络请求成功，收到返回
// nResult : 返回值
- (void) onUCSHttpRequestDidFinishLoading:(UCSHttpRequest *) pUCSHttpRequest withData:(NSData *) data withResult:(int)nResult
{
    NSMutableDictionary *returnDic = [NSMutableDictionary dictionary];
    if (!data) {
        return;
    }
    [returnDic setDictionary:[NSJSONSerialization JSONObjectWithData:data options:0 error:nil]];
    
//    NSLog(@"static/address-->%@", returnDic);
    
    NSDateFormatter *formatter = [[[NSDateFormatter alloc] init] autorelease];
    [formatter setTimeZone:[NSTimeZone localTimeZone]];
    formatter.dateFormat = @"yyyy-MM-dd";
    NSString *currDateStr = [formatter stringFromDate:[NSDate date]];
    
    
    if (m_pUCSHttpRequestSvrAddr == pUCSHttpRequest)
    {
        switch (nResult)
        {
            case 0:
            {
                [UserDefaultManager SetLocalDataString:currDateStr key:DATA_STORE_GET_STATIC_ADDR_TIME];
                
                
                // httpserver
                NSArray *arrayHttpServerList = [returnDic objectForKey:@"httpserver"];
                [UserDefaultManager SetLocalDataObject:arrayHttpServerList key:DATA_STORE_HTTPSERVER_LIST];
                
                
                // rtpplist
                NSArray *rtpList = [returnDic objectForKey:@"rtpp"];
                if (rtpList != nil && [rtpList isKindOfClass:[NSArray class]] && [rtpList count] != 0)
                {
                    [UserDefaultManager SetLocalDataObject:rtpList key:DATA_STORE_RTPPLIST];
                    
                    [[SoftphoneManager instance] setDefaultRttp];
                    //开启rtpp udp ping检测
                    [[SoftphoneManager instance] startVoipListPing];
                    
                } else{
                    [UserDefaultManager SetLocalDataObject:nil key:DATA_STORE_RTPPLIST];
                }
                
                
                // stunServerlist
                NSArray *stunServerlist = [returnDic objectForKey:@"stun"];
                if (stunServerlist != nil && [stunServerlist isKindOfClass:[NSArray class]] && [stunServerlist count] != 0)
                {
                    [UserDefaultManager SetLocalDataObject:stunServerlist key:DATA_STORE_STUNLIST];
                } else{
                    [UserDefaultManager SetLocalDataObject:nil key:DATA_STORE_STUNLIST];
                }
                
                
            }
                break;
                
            default:{
            }
                break;
        }
    }
}

// 网络请求出现错误
- (void) onUCSHttpRequestDidFailWithError:(UCSHttpRequest *) pUCSHttpRequest withError:(NSError *) error
{

   if (m_pUCSHttpRequestSvrAddr == pUCSHttpRequest)
    {
        NSLog(@"UCSNetStaticInterface -- m_pUCSHttpRequestSvrAddr -  : %d", [error code]);
    }
    
}

@end
