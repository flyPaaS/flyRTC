//
//  UCSQualityRequestManager.m
//  KCTApiSdk
//
//  Created by wenqinglin on 14-4-9.
//  Copyright (c) 2014年 ucs. All rights reserved.
//

#import "UCSQualityRequestManager.h"

#import "Utility.h"
#import "UserDefaultManager.h"
#import "UCSStatisticalHelp.h"


#define REPORTDATATYPE @"reportDataType"


@implementation UCSQualityRequestManager

static UCSQualityRequestManager *g_instance = nil;


+(UCSQualityRequestManager*)instance{

    if (nil==g_instance) {
        g_instance = [[UCSQualityRequestManager alloc] init];
    }
    
    return  g_instance;
    
}



// 初始化
- (id) init
{
    self = [super init];
    if (self) {
        m_delegate = nil;
        
        self.m_pUCSASIHTTPRequest = nil;
 
 
        
    }
    return self;
}

// dealloc
- (void) dealloc
{

    self.m_pUCSASIHTTPRequest = nil;

    
    [super dealloc];
}

// 停止网络请求
- (void) stopRequest
{
    if (nil != self.m_pUCSASIHTTPRequest)
    {
        [self.m_pUCSASIHTTPRequest clearDelegatesAndCancel];
    }
}

- (void) startAsynchronousWithURL:(NSURL *) url
{
  
    
    self.m_pUCSASIHTTPRequest = [UCSASIHTTPRequest requestWithURL:url];
    [self.m_pUCSASIHTTPRequest setUseCookiePersistence:YES];
 
    
    // 设定委托，委托自己实现异步请求方法
//    char ch[100] = {0};
//    Security((unsigned char*)&ch[0]);
//    [self.m_pUCSASIHTTPRequestLogIn addRequestHeader:@"SecurityFlag" value:[NSString stringWithCString:ch encoding:NSUTF8StringEncoding]];
    
    [self.m_pUCSASIHTTPRequest setDelegate:self];
    // 开始异步请求
    [self.m_pUCSASIHTTPRequest startAsynchronous];
}

//获取时间戳 格式为:20140814104428 
- (NSString*) gettimeSp:(NSDate*)datenow
{
    NSDateFormatter *formatter=[[[NSDateFormatter alloc] init] autorelease];
    [formatter setTimeZone:[NSTimeZone localTimeZone]];
    [formatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    NSString *myDate = [formatter stringFromDate:datenow];
    
    NSArray *ymdhms = [myDate componentsSeparatedByString:@" "];
    
    NSArray *ymd = [ymdhms[0] componentsSeparatedByString:@"-"];
    
    NSArray *hms = [ymdhms[1] componentsSeparatedByString:@":"];
    
    NSString *timeSp = [NSString stringWithFormat:@"%@%@%@%@%@%@",ymd[0], ymd[1],ymd[2],hms[0],hms[1],hms[2]];
    
    return timeSp;

}



 
 




#pragma mark 上报手机数据
//上报通话质量
-(void) reportQualityForSP:(NSDictionary*)qualData;
{
    
        NSString *reportType = @"reportQuality";

    
    NSDictionary *reportData = [qualData retain] ;

    

    if ([NSJSONSerialization isValidJSONObject:reportData])
    {
    
        NSError *error;
        NSData *jsonData = [NSJSONSerialization dataWithJSONObject:reportData options:NSJSONWritingPrettyPrinted error: &error];
        NSMutableData *tempJsonData = [NSMutableData dataWithData:jsonData];
        
        
        NSString *jsonString =[[NSString alloc] initWithData:tempJsonData encoding:NSUTF8StringEncoding];
        NSLog(@"Register JSON:%@",jsonString);
    
    
        
        NSURL *url= nil;
        
        if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"TESTMODE"])//测试
        {
             url = [NSURL URLWithString:[NSString stringWithFormat:@"http://113.31.89.144:8088/ulog/log?event=quality&uid=%@",[UserDefaultManager GetclientNumber]]];
        }
        else if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"DEVMODE"])//DEV 模式
        {
//            url = [NSURL URLWithString:[NSString stringWithFormat:@"http://113.31.89.144:8088/ulog/log?event=quality&uid=%@",[UserDefaultManager GetclientNumber]]]; //开发环境
        }
        else //正式
        {
            url = [NSURL URLWithString:[NSString stringWithFormat:@"http://ulog.ucpaas.com/ulog/log?event=quality&uid=%@",[UserDefaultManager GetclientNumber]]];
        }
        
        
        
        //包头
//        NSString *uid  = [UserDefaultManager GetUserID];
//        
//        NSInteger nSnRandNum = rand()+[uid integerValue];//为防止随机码重复故回拨时加随机码＋uid。
//        
//        NSString *strnSnRandNum = [NSString stringWithFormat:@"%.0f",fabs(nSnRandNum)];
//        
//        NSString *sign_md5 = [[UxinUtility shareUtility] md5String:[NSString stringWithFormat:@"%@%@%@",strnSnRandNum,uid,PUBLIC_KEY]];
        
        //包头
 
 
 

    

 
    UCSASIHTTPRequest *request = [UCSASIHTTPRequest requestWithURL:url];
    
//    [request setValidatesSecureCertificate:NO];//请求为HTTPS时需要设置这个属性
    
    [request addRequestHeader:@"Content-Type" value:@"application/json; encoding=utf-8"];
    [request addRequestHeader:@"Accept" value:@"application/json"];
//    [request addRequestHeader:@"Authorization" value:auth_base64];
        
//    [request addRequestHeader:@"uid" value: uid];
//    [request addRequestHeader:@"sn" value:strnSnRandNum];
//    [request addRequestHeader:@"sign" value: sign_md5];
        
    [request setRequestMethod:@"POST"];
    [request setPostBody:tempJsonData];
    [request setDelegate:self];
        
    NSDictionary *httpDic = [NSDictionary dictionaryWithObjectsAndKeys:reportType,REPORTDATATYPE,nil];
        
    request.userInfo = httpDic;
    self.m_pUCSASIHTTPRequest = request;
    [request startAsynchronous];
    
    
        
        [jsonString release];
    
    }
    
    [reportData release];




}



 




#pragma mark - UCSASIHTTPRequestDelegate methods

// 请求失败，获取 error
- ( void )requestFailed:( UCSASIHTTPRequest *)request
{
    if (request == self.m_pUCSASIHTTPRequest)
    {

        if (nil != m_delegate && [m_delegate respondsToSelector:@selector(onLoginDidFailWithError:)])
        {
            [m_delegate onLoginDidFailWithError:[request error]];
        }
    }
}


// 请求结束，获取 Response 数据
- (void) requestFinished:(UCSASIHTTPRequest *)request
{
    if (request == self.m_pUCSASIHTTPRequest)
    {
        NSData *m_webData = [request responseData];
        NSMutableDictionary *returnDic = [NSMutableDictionary dictionary];
        [returnDic setDictionary:[NSJSONSerialization JSONObjectWithData:m_webData options:0 error:nil]];
        NSLog(@"----------------UCSQualityRequestManager-------------requestFinished:%@",returnDic);

//        NSMutableDictionary *resultDic =[returnDic objectForKey:@"resp"];
      
        
        



        int nRet = -1;//HTTP_REQUEST_SERVER_ERROR;
 

         if ([[request.userInfo objectForKey:REPORTDATATYPE] isEqualToString:@"phoneInfo"])
        {
            
            
            
            if ([returnDic count] > 0)
            {
                nRet = [[returnDic objectForKey:@"code"] intValue];
                
                switch (nRet)
                {
                    case 0: // success
                    {
                        
                        //上报手机信息成功
                        
                        
                        
                    }
                        break;
                        
                    default:
                        break;
                }
            }
        
        
        }
         else if ([[request.userInfo objectForKey:REPORTDATATYPE] isEqualToString:@"errorCodeInfo"])
        {
         
            if ([returnDic count] > 0)
            {
                nRet = [[returnDic objectForKey:@"code"] intValue];
                
                switch (nRet)
                {
                    case 0: // success
                    {
                        
                        //上报错误码信息成功
                        
                        
                        
                    }
                        break;
                        
                    default:
                        break;
                }
            }
            
            
        }
        
        
        
        if (nil != m_delegate && [m_delegate respondsToSelector:@selector(onLoginDidFinishLoading:)])
        {

            [m_delegate performSelector:@selector(onLoginDidFinishLoading:) withObject:nRet];
        }
    }
}

// save the session string
- (void)request:(UCSASIHTTPRequest *)request didReceiveResponseHeaders:(NSDictionary *)responseHeaders
{
    if (request == self.m_pUCSASIHTTPRequest)
    {
        NSString *cookie = [responseHeaders valueForKey:@"Set-Cookie"]; // It is your cookie
        cookie = [cookie stringByReplacingOccurrencesOfString:@"ssid=" withString:@""];
        cookie = [cookie stringByReplacingOccurrencesOfString:@"; Path=/" withString:@""];
        
//        [UserDefaultManager SetLocalDataString:cookie key:DATA_STORE_COOKIE];
        
//        NSLog(@"The session string is : %@", cookie);
    }
}
@end
