//
//  CallLogManager.mm
//  UXinClient
//
//  Created by Liam on 13-5-30.
//  Copyright (c) 2013年 UXIN. All rights reserved.
//

#import "CallLogManager.h"
#import "UCSASIHTTPRequest.h"
#import "UCSASIFormDataRequest.h"
#import "SoftphoneManager.h"


// 电话日志上传相关
// 获取电话日志上传配置参数
#define URL_CALL_LOG_CONFIG                 @"http://omp.guoling.com/reportcontrol.action?bid=%@&pv=%@&v=%@&sipv=%@"
// 上传电话日志
#define URL_CALL_LOG_COMMIT                 @"http://omp.guoling001.com/reportlist.action"

CallLogManager *g_pCallLogManager = nil;

@implementation CallLogManager
@synthesize bNeedUploadCalllog;
@synthesize nMaxSize;
@synthesize strCalled;
@synthesize dicUploadInfo;

+ (CallLogManager *) instance
{
    if (nil == g_pCallLogManager)
    {
        g_pCallLogManager = [[self alloc] init];
    }
    
    return g_pCallLogManager;
}

- (id) init
{
    self = [super init];
    if (self)
    {
        m_pUCSHttpRequestGetCallLogConfig = nil;
        self.bNeedUploadCalllog = NO;
        self.nMaxSize = 0;
        self.strCalled = @"";
        nSize = 0;
        self.dicUploadInfo = nil;
        bSubmiting = NO;
    }
    
    return self;
}

- (void) dealloc
{
    UXIN_HTTP_RELEASE(m_pUCSHttpRequestGetCallLogConfig);
    self.strCalled = nil;
    self.dicUploadInfo = nil;

    [super dealloc];
}

- (NSString *) getPV
{
    NSString *strPV = @"iphone-app";
    if (kUseBreakprison)
    {
        strPV = @"iphone";
    }
    
    return strPV;
}

// 获取电话日志上传配置
- (void) getCallLogConfig
{
    BOOL bNeedGet = NO;
    
    NSString *strTime = [UserDefaultManager GetLocalDataString:DATA_STORE_GET_CALLLOG_CONFIG_TIME];
    if (nil != strTime)
    {
        NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
        [formatter setTimeZone:[NSTimeZone localTimeZone]];
        formatter.dateFormat = @"yyyy-MM-dd";
        NSString *currDateStr = [formatter stringFromDate:[NSDate date]];
        
        if (![strTime isEqualToString:currDateStr])
        {
            bNeedGet = YES;
        }
        [formatter release];
    }
    else
    {
        bNeedGet = YES;
    }
    
    NSString *strUrl = [NSString stringWithFormat:URL_CALL_LOG_CONFIG, @"uxin", [self getPV], kVersion, [[SoftphoneManager instance] getSoftphoneVersion]];
    
    if (bNeedGet)
    {
        UXIN_HTTP_RELEASE(m_pUCSHttpRequestGetCallLogConfig);
        m_pUCSHttpRequestGetCallLogConfig = [[UCSHttpRequest alloc] initWithRequestStyle:NO isWillReturnJson:YES];
        [m_pUCSHttpRequestGetCallLogConfig setURLAndDelegate:strUrl withDelegate:self];
        [m_pUCSHttpRequestGetCallLogConfig startAsyncRequest];
    }
}

// 判断是否需要上传电话日志
- (void) determineIsNeedUploadCallLog:(NSMutableDictionary *) configDic
{
    NSMutableDictionary *tempDic = [configDic copy];
    int nUpflag = [[tempDic objectForKey:@"upflag"] intValue];
    nSize = [[tempDic objectForKey:@"size"] intValue];
    
    self.nMaxSize = nSize;
    
    if (1 == nUpflag)
    {
        self.bNeedUploadCalllog = YES;
    }
    else
    {
        self.bNeedUploadCalllog = NO;
    }
    
    [tempDic release];
}

- (void) saveCallLogInfo:(NSString *) summary withDetail:(NSString *) detail
{
    NSLog(@"%@ -- %@", summary, detail);
    
    if (bSubmiting)
    {
        return;
    }
    
    if (nil == self.dicUploadInfo)
    {
        NSMutableDictionary *tempDic = [[NSMutableDictionary alloc] init];
        self.dicUploadInfo = tempDic;
        [tempDic release];
    }
    
    NSString *strSRC = [NSString stringWithFormat:@"%@_%@_%@_%@_%@", @"UXinClient", [NSString stringWithFormat:@"%@%@", [[UIDevice currentDevice] systemName], [[UIDevice currentDevice] systemVersion]], kPhoneType, kVersion, @"UXinClient"];
    NSString *strCategory = @"softphone_error";
    NSString *strSummary = nil;
    if (nil != summary)
    {
        strSummary = [summary retain];
    }
    else
    {
        strSummary = [[NSString alloc] initWithString:@""];
    }
    
    NSString *strDetail = @"";
    if (nil != detail)
    {
        strDetail = [detail retain];
    }
    else
    {
        strDetail = [[NSString alloc] initWithString:@""];
    }
    NSMutableDictionary *dicListItem = [NSMutableDictionary dictionary];
    [dicListItem setObject:strSRC forKey:@"src"];
    [dicListItem setObject:strCategory forKey:@"category"];
    [dicListItem setObject:strSummary forKey:@"summary"];
    [dicListItem setObject:strDetail forKey:@"detail"];
    
    NSMutableDictionary *dicContent = [NSMutableDictionary dictionary];
    [dicContent setObject:@"uxin" forKey:@"bid"];
    [dicContent setObject:[UserDefaultManager GetUserID] forKey:@"uid"];
    [dicContent setObject:[self getPV] forKey:@"pv"];
    [dicContent setObject:kVersion forKey:@"v"];
    [dicContent setObject:[UserDefaultManager GetUserMobile] forKey:@"call"];
    [dicContent setObject:self.strCalled forKey:@"called"];
    
    NSMutableArray *arrayListInfo = [self.dicUploadInfo objectForKey:@"list"];
    if (nil != arrayListInfo)
    {
        [arrayListInfo addObject:dicListItem];
    }
    else
    {
        arrayListInfo = [NSMutableArray array];
        [arrayListInfo addObject:dicListItem];
    }
    [self.dicUploadInfo setObject:arrayListInfo forKey:@"list"];
    [self.dicUploadInfo setObject:dicContent forKey:@"content"];
    
    [strSummary release];
    [strDetail release];
}

- (void) submitLogToServer
{
    [self performSelector:@selector(realSubmit) withObject:nil afterDelay:1.0f];
}


- (void) realSubmit
{
    bSubmiting = YES;
    if (nil != self.dicUploadInfo)
    {
        NSString *strBody = [self.dicUploadInfo JSONRepresentation];
        if ((nSize > 0 && (nSize >= [strBody length] / 2)) || 0 == nSize)
        {
            UCSASIFormDataRequest *pUCSASIHTTPRequest = [UCSASIFormDataRequest requestWithURL:[NSURL URLWithString:URL_CALL_LOG_COMMIT]];
            NSMutableDictionary *dicContent = [self.dicUploadInfo objectForKey:@"content"];
            if (nil != dicContent)
            {
                [pUCSASIHTTPRequest setPostValue:dicContent forKey:@"content"];
            }
            NSMutableArray *arrayListInfo = [self.dicUploadInfo objectForKey:@"list"];
            if (nil != arrayListInfo)
            {
                [pUCSASIHTTPRequest setPostValue:arrayListInfo forKey:@"list"];
            }
            // 设定委托，委托自己实现异步请求方法
            [pUCSASIHTTPRequest setDelegate:self];
            [pUCSASIHTTPRequest startAsynchronous];
        }
        
        self.dicUploadInfo = nil;
    }
    bSubmiting = NO;
}

#pragma mark-
#pragma mark UCSASIHTTPRequestDelegate methods
// 请求结束，获取 Response 数据
- (void) requestFinished:(UCSASIHTTPRequest *) request
{
    NSData *responseData = [request responseData];
    
    if (nil == responseData)
    {
        return;
    }
    
    NSMutableDictionary *returnDic = [NSMutableDictionary dictionary];
    NSString *jsonValue = [[NSString alloc] initWithData:responseData encoding:NSUTF8StringEncoding];
    [returnDic setDictionary:[jsonValue JSONValue]];
    [jsonValue release];
    
    bSubmiting = NO;
    
    NSLog(@"Submit Call log success");
}

// 请求失败，获取 error
- (void) requestFailed:(UCSASIHTTPRequest *) request
{
    bSubmiting = NO;
    
    NSLog(@"Call commit net error:%@", [[request error] description]);
}


#pragma mark-
#pragma mark UCSHttpRequestDelegate methods
// 网络请求成功，收到返回
// nResult : 返回值
- (void) onUCSHttpRequestDidFinishLoading:(UCSHttpRequest *) pUCSHttpRequest withData:(NSData *) data withResult:(int)nResult
{
    NSString *jsonValue = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    NSMutableDictionary *returnDic = [NSMutableDictionary dictionary];
    [returnDic setDictionary:[jsonValue JSONValue]];
    [jsonValue release];
    
    if (pUCSHttpRequest == m_pUCSHttpRequestGetCallLogConfig)
    {
//        NSLog(@"%@", returnDic);
        if (0 == nResult)
        {
            NSMutableDictionary *configDic = [returnDic objectForKey:@"config"];
            if (nil != configDic)
            {
                NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
                [formatter setTimeZone:[NSTimeZone localTimeZone]];
                formatter.dateFormat = @"yyyy-MM-dd";
                NSString *currDateStr = [formatter stringFromDate:[NSDate date]];
                [UserDefaultManager SetLocalDataString:currDateStr key:DATA_STORE_GET_CALLLOG_CONFIG_TIME];
                [formatter release];
                
                [self determineIsNeedUploadCallLog:configDic];
            }
        }
        else
        {
            NSLog(@"Get Call log config failed(%d)", nResult);
        }
    }
}

// 网络请求出现错误
- (void) onUCSHttpRequestDidFailWithError:(UCSHttpRequest *) pUCSHttpRequest withError:(NSError *) error
{
    NSLog(@"Get Call log config failed with error:%@", [error description]);
}

@end
