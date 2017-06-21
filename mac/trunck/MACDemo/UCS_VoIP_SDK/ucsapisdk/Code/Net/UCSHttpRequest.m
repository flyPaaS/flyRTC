//
//  UCSHttpRequest.m
//  UXinClient
//
//  Created by Liam Peng on 12-5-8.
//  Copyright (c) 2012年 UXIN. All rights reserved.
//

#import "UCSHttpRequest.h"
#import "UCSASIFormDataRequest.h"
#import "SecurityURLAdaptor.h"

#define ID_MSGBOX_PWD_ERROR                 1000        // 密码错误提示框ID


@implementation UCSHttpRequest
@synthesize isNeedLoginWhenSessionOutDate;
@synthesize delegate = m_delegate;

- (id) initWithRequestStyle:(BOOL) bPost isWillReturnJson:(BOOL) bJson
{
    self = [super init];
    
    if (self)
    {
        m_delegate = nil;
        m_pUCSASIHTTPRequest = nil;
        m_bPost = bPost;
        m_bLogined = NO;
        m_bWillReturnJson = bJson;
        self.isNeedLoginWhenSessionOutDate = YES;
    }
    
    return self;
}

- (void) dealloc
{
    [self stopRequest];
    
    self.m_strURL = nil;
    UXIN_MEM_RELEASE(m_pUCSASIHTTPRequest);
    
    self.isNeedLoginWhenSessionOutDate = YES;
    
    [super dealloc];
}

// 设置URL及delegate
- (void) setURLAndDelegate:(NSString *) strURL withDelegate:(id) delegate
{
    if (strURL && [strURL isKindOfClass:[NSString class]] && [strURL length] > 0)
    {
        self.m_strURL = strURL;
    }
    
    
    m_delegate = delegate;
    
    if (nil == self.m_strURL)
    {
        return;
    }
    
    NSURL *url = [NSURL URLWithString:self.m_strURL];
    //    BOOL security = [[SecurityURLAdaptor sharedAdaptor] isSupport:url];
    //    if (security) {
    ////        NSLog(@"Support url: %@", [url absoluteString]);
    //        if (m_bPost == NO) {
    //            // POST 模式都是外部自己计算表单，里面不能计算
    //            url = [[SecurityURLAdaptor sharedAdaptor] convert:url];
    //            self.m_strURL = [url absoluteString];
    //        }
    //    }
    
    //    if (m_bPost)//UCSHttpRequest默认是用GET请求
    //    {
    //        m_pUCSASIHTTPRequest = [[UCSASIFormDataRequest requestWithURL:url] retain];// POST
    //    }
    //    else
    //    {
    m_pUCSASIHTTPRequest = [[UCSASIHTTPRequest requestWithURL:url] retain];// GET
    //    }
    
    
    //    Security((unsigned char*)&ch[0]);
    //    [pRequest addRequestHeader:@"SecurityFlag" value:[NSString stringWithCString:ch encoding:NSUTF8StringEncoding]];
    
    // 设定委托，委托自己实现异步请求方法
    [m_pUCSASIHTTPRequest setDelegate:self];
    
    m_bLogined = NO;
}

- (void) setURLAndDelegate2:(NSString *) strURL withDelegate:(id) delegate
{
    if (strURL && [strURL isKindOfClass:[NSString class]] && [strURL length] > 0)
    {
        self.m_strURL = strURL;
    }
    

    m_delegate = delegate;
    
    if (nil == self.m_strURL)
    {
        return;
    }
    
    NSURL *url = [NSURL URLWithString:self.m_strURL];
//    BOOL security = [[SecurityURLAdaptor sharedAdaptor] isSupport:url];
//    if (security) {
////        NSLog(@"Support url: %@", [url absoluteString]);
//        if (m_bPost == NO) {
//            // POST 模式都是外部自己计算表单，里面不能计算
//            url = [[SecurityURLAdaptor sharedAdaptor] convert:url];
//            self.m_strURL = [url absoluteString];
//        }
//    }
    
//    if (m_bPost)//UCSHttpRequest默认是用GET请求
//    {
//        m_pUCSASIHTTPRequest = [[UCSASIFormDataRequest requestWithURL:url] retain];// POST
//    }
//    else
//    {
        m_pUCSASIHTTPRequest = [[UCSASIHTTPRequest requestWithURL:url] retain];// GET
//    }
    
    
//    Security((unsigned char*)&ch[0]);
//    [pRequest addRequestHeader:@"SecurityFlag" value:[NSString stringWithCString:ch encoding:NSUTF8StringEncoding]];
    

    
    
    [UCSASIHTTPRequest setSessionCookies:nil];
//    [m_pUCSASIHTTPRequest addRequestHeader:@"ac" value:[UserDefaultManager GetAC]];
    [m_pUCSASIHTTPRequest addRequestHeader:@"ac" value: [UserDefaultManager GetIMSSID]];
    
    // ----------------------------------------------------------------------------
    // Cookie
    
    NSString *ssid = [UserDefaultManager GetLocalDataString:DATA_STORE_COOKIE];
    if (ssid == nil) {
        ssid = @"";
    }
    NSDictionary *properties = [[[NSMutableDictionary alloc] init] autorelease];
    [properties setValue:ssid forKey:NSHTTPCookieValue];
    [properties setValue:@"ssid" forKey:NSHTTPCookieName];
    [properties setValue:@"ucpaas.com" forKey:NSHTTPCookieDomain];
    [properties setValue:@"ucpaas.com" forKey:NSHTTPCookieOriginURL];
    [properties setValue:[NSDate dateWithTimeIntervalSinceNow:60*60] forKey:NSHTTPCookieExpires];
    [properties setValue:@"/" forKey:NSHTTPCookiePath];
    NSHTTPCookie *cookie = [[[NSHTTPCookie alloc] initWithProperties:properties] autorelease];
    
    [m_pUCSASIHTTPRequest setUseCookiePersistence:NO];
    [m_pUCSASIHTTPRequest setRequestCookies:[NSMutableArray arrayWithObject:cookie]];
    
    // ----------------------------------------------------------------------------
    
//    if (security) {
    
    char ch[100] = {0};
    Security((unsigned char*)&ch[0]);
    [m_pUCSASIHTTPRequest addRequestHeader:@"SecurityFlag" value:[NSString stringWithCString:ch encoding:NSUTF8StringEncoding]];
//    }
    // 设定委托，委托自己实现异步请求方法
    [m_pUCSASIHTTPRequest setDelegate:self];
    
    m_bLogined = NO;
}

- (void) startAsyncRequest {
    if (nil != m_pUCSASIHTTPRequest)
    {
//        NSLog(@"startAsyncRequest = %@", self.m_strURL);
        // 开始异步请求
        [m_pUCSASIHTTPRequest startAsynchronous];   
    }
}

- (void) stopRequest
{
    m_delegate = nil;
    if (nil != m_pUCSASIHTTPRequest) 
    {
        [m_pUCSASIHTTPRequest clearDelegatesAndCancel];
    }
    
    
}


// 提示密码错误提示框
- (void) showPasswordErrorMsg
{
//    [SVProgressHUD dismiss];
    
//    [[UXTCPManager instance] logout];

    [PublicFunc ShowMessageBox:ID_MSGBOX_PWD_ERROR titleName:nil contentText:@"密码错误，请重新登录。" cancelBtnName:@"我知道了" delegate:self];
}

- (void)addRequestHeader:(NSString *)header value:(NSString *)value
{
    if (m_pUCSASIHTTPRequest)
    {
        [m_pUCSASIHTTPRequest addRequestHeader:header value:value];
    }
}

// set PostValue
- (void) setPostValue:(id <NSObject>)value forKey:(NSString *)key
{
    [(UCSASIFormDataRequest *)m_pUCSASIHTTPRequest setPostValue:value forKey:key];
}

- (void)addPostData:(id)data withFileName:(NSString *)fileName andContentType:(NSString *)contentType forKey:(NSString *)key
{
    [(UCSASIFormDataRequest *)m_pUCSASIHTTPRequest addData:data withFileName:fileName andContentType:contentType forKey:key];
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
    
    int nRet = HTTP_REQUEST_SERVER_ERROR;
    if (m_bWillReturnJson)
    {
        NSMutableDictionary *returnDic = [NSMutableDictionary dictionary];
        id res = [NSJSONSerialization JSONObjectWithData:responseData options:0 error:nil];
        if (nil != res && [res isKindOfClass:[NSDictionary class]])
        {
            [returnDic setDictionary:res];
        }else {
            UXLogError(@"HTTP RESPONS ERROR: %@", res);
            responseData = nil;
        }
        
        
        if ([returnDic count] > 0)
        {
            // 这里返回session过期或者未登录都重新登录，再发起请求一次 存放RTPP地址静态的静态文件。
//            nRet = [[returnDic objectForKey:@"result"] intValue];
              nRet = 0; //如果json返回正确数据则赋值为0。
            switch (nRet)
            {
                case -1: // 未登录
                {
                    if (!m_bLogined && self.isNeedLoginWhenSessionOutDate)
                    {
                        NSLog(@"not login");
                        return;
                    }
                }
                    break;
                case 10: // session过期
                {
                    if (!m_bLogined && self.isNeedLoginWhenSessionOutDate)
                    {
                        NSLog(@"session outdate");
                        return;
                    }
                }
                    break;
                default:
                    break;
            }
        }
        else
        {
            NSLog(@"error!!can get the server return data.");
        }   
    }
    else
    {
        nRet = 0;
    }

    if (nil != m_delegate && [m_delegate respondsToSelector:@selector(onUCSHttpRequestDidFinishLoading:withData:withResult:)]) 
    {
        TIME_ANALYZE_BEGINMAIN( 10, 0, "UCSHttpRequest - requestFinished" );
        [m_delegate onUCSHttpRequestDidFinishLoading:self withData:responseData withResult:nRet];
        TIME_ANALYZE_ENDMAIN( 10, 0 );
    }
}

// 请求失败，获取 error
- (void) requestFailed:(UCSASIHTTPRequest *) request
{
    NSError *error = [request error];
    
    if (nil != m_delegate && [m_delegate respondsToSelector:@selector(onUCSHttpRequestDidFailWithError:withError:)])
    {
        TIME_ANALYZE_BEGINMAIN( 11, 0, "UCSHttpRequest - requestFailed" );
        [m_delegate onUCSHttpRequestDidFailWithError:self withError:error];
        TIME_ANALYZE_ENDMAIN( 11, 0 );
    }
}

#pragma mark-
#pragma UCSLoginManagerDelegate Methods
// 网络请求成功，收到返回
// nResult : 返回值
- (void) onLoginDidFinishLoading:(int) nResult
{
    switch (nResult)
    {
        case 0: // success
        {
            NSString *strUrl = [[[NSString alloc] initWithString:self.m_strURL] autorelease];
            [self setURLAndDelegate:strUrl withDelegate:m_delegate];
            m_bLogined = YES;
            // 再次发送呼叫请求
            [self startAsyncRequest];
        }
            break;
        case 4: // 用户名或密码错误
        {
            [self showPasswordErrorMsg];
        }
            break;
        default:
        {
            NSLog(@"Login Error : %d", nResult);
            if (nil != m_delegate && [m_delegate respondsToSelector:@selector(onUCSHttpRequestDidFinishLoading:withData:withResult:)])
            {
                TIME_ANALYZE_BEGINMAIN( 12, 0, "UCSHttpRequest - onLoginDidFinishLoading" );
                [m_delegate onUCSHttpRequestDidFinishLoading:self withData:nil withResult:nResult];
                TIME_ANALYZE_ENDMAIN( 12, 0 );
            }
        }
            break;
    }
}

// 网络请求出现错误
- (void) onLoginDidFailWithError:(NSError *) error
{
    NSLog(@"login error %@", error.userInfo);
    
    if (nil != m_delegate && [m_delegate respondsToSelector:@selector(onUCSHttpRequestDidFailWithError:withError:)])
    {
        TIME_ANALYZE_BEGINMAIN( 13, 0, "UCSHttpRequest - onLoginDidFailWithError" );
        [m_delegate onUCSHttpRequestDidFailWithError:self withError:error];
        TIME_ANALYZE_ENDMAIN( 13, 0 );
    }
}

@end
