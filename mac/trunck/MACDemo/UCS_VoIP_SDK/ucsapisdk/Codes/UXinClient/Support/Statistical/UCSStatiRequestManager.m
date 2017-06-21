//
//  UCSStatiRequestManager.m
//  KCTApiSdk
//
//  Created by wenqinglin on 14-4-9.
//  Copyright (c) 2014年 ucs. All rights reserved.
//

#import "UCSStatiRequestManager.h"

#import "Utility.h"
#import "UserDefaultManager.h"
#import "UCSStatisticalHelp.h"

#define REPORTDATATYPE @"reportDataType"


@implementation UCSStatiRequestManager

static UCSStatiRequestManager *g_instance = nil;


+(UCSStatiRequestManager*)instance{

    if (nil==g_instance) {
        g_instance = [[UCSStatiRequestManager alloc] init];
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



 
//拼装上报手机数据
- (NSDictionary *) getReportPhoneData:(NSString*)clientNumber{


    //拼接请求地址
    
    NSString * str_operator = [[UCSStatisticalHelp instance] getMCCAndMNCInfo];
    NSString * str_brand = [[UCSStatisticalHelp instance] getPhoneBrand];
    NSString * str_model = [[UCSStatisticalHelp instance] getplatformString];
    NSString * str_imei = [[UCSStatisticalHelp instance] getIMEI];//不允许访问
    NSString * str_imsi = [[UCSStatisticalHelp instance] getIMSI];//不允许访问
    NSString * str_width = [[UCSStatisticalHelp instance] getScreenWidth];
    NSString * str_height = [[UCSStatisticalHelp instance] getScreenHeight];
    NSString * str_mac = [[UCSStatisticalHelp instance] getMacAddress]; //6。0可用，7.0返回固定值
    NSString * str_cpu = [[UCSStatisticalHelp instance] getCPUType];
    NSString * str_cpurate = [[UCSStatisticalHelp instance] getCpuFrequency];
    NSString * str_memory = [NSString stringWithFormat:@"%u",[[UCSStatisticalHelp instance] getTotalMemoryBytes]] ;
    NSString * str_os = [[UCSStatisticalHelp instance] getSystemName];
    NSString * str_version = [[UCSStatisticalHelp instance] getSystemVersion];
    NSString * str_sdkVersion = [[UCSStatisticalHelp instance] getSDKVersion];
    NSString * str_demoVersion = [[UCSStatisticalHelp instance] getAppVersion];
    NSString * str_packageName = [[UCSStatisticalHelp instance] getpackageName];
//    NSString * str_clientNumber = [[UCSStatisticalHelp instance] getclientNumber];
    NSString * str_logDate = [[UCSStatisticalHelp instance] getlogDate];
    
    
    //modify for mac
    /*
    NSDictionary *tempreportData = [[[NSDictionary alloc] initWithObjectsAndKeys:str_operator, @"operator",str_brand, @"brand",str_model, @"model",str_imei, @"imei",str_imsi, @"imsi",str_width, @"width",str_height, @"height",str_mac, @"mac",str_cpu, @"cpu",str_cpurate, @"cpu_rate",str_memory, @"memory",str_os, @"os",str_version, @"version",str_sdkVersion, @"sdkVersion",str_demoVersion, @"demoVersion",str_packageName, @"packageName",clientNumber, @"clientNumber",str_logDate, @"logDate", nil] autorelease];
     */
    
    return nil;

}





#pragma mark 上报手机数据
//上报手机数据
- (void) reportPhoneInfoForStati:(NSString*)clientNumber{
    
    NSString *reportType = @"phoneInfo";
    
    
    NSDictionary *reportData = [[self getReportPhoneData:clientNumber] retain];

    
    
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
            url = [NSURL URLWithString:[NSString stringWithFormat:@" http://113.31.89.144:8088/ulog/log?event=mobileLog"]];
        }
        else if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"DEVMODE"])//DEV 模式
        {
               //开发环境
//             url = [NSURL URLWithString:[NSString stringWithFormat:@" http://113.31.89.144:8088/ulog/log?event=mobileLog"]];
        }
        else //正式
        {
            url = [NSURL URLWithString:[NSString stringWithFormat:@"http://ulog.ucpaas.com/ulog/log?event=mobileLog"]];
        }
    


 
    UCSASIHTTPRequest *request = [UCSASIHTTPRequest requestWithURL:url];
    
//    [request setValidatesSecureCertificate:NO];//请求为HTTPS时需要设置这个属性
    
    [request addRequestHeader:@"Content-Type" value:@"application/json; encoding=utf-8"];
    [request addRequestHeader:@"Accept" value:@"application/json"];
//    [request addRequestHeader:@"Authorization" value:auth_base64];
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


//拼装要上报的错误码数据
- (NSMutableArray *) getReportErrorcodeData:(NSString*)interfaceName withinterfaceDesc:(NSString*)interfaceDesc withErrorCode:(NSString*)errorCode withErrorDesc:(NSString*)errorDesc withClientNumber:(NSString*)clientNumber{
    
    
        NSString * str_logDate = [[UCSStatisticalHelp instance] getlogDate];
        NSString * str_iftype = [NSString stringWithFormat:@"%d",1];//1.sdk 2.rest

    
    NSDictionary *tempreportData = [[[NSDictionary alloc] initWithObjectsAndKeys:clientNumber, @"clientNumber",str_iftype, @"ifType",interfaceName, @"ifName",errorCode, @"errorCode",errorDesc, @"errorMsg",str_logDate, @"logDate",  nil] autorelease];
    
    
        NSMutableArray *reportDataArray = [[[NSMutableArray alloc]init] autorelease];
    
        [reportDataArray addObject:tempreportData];
    
//        NSLog(@"Register JSON:%@",[NSString stringWithFormat:@"%@",reportDataArray]);
    
    return reportDataArray;

}

#pragma mark 上报错误码数据
//上报错误码数据
- (void) reportErrorCodeInfoForStati:(NSString*)interfaceName withinterfaceDesc:(NSString*)interfaceDesc withErrorCode:(NSString*)errorCode withErrorDesc:(NSString*)errorDesc withClientNumber:(NSString*)clientNumber{

    NSString *reportType = @"errorCodeInfo";
    
    NSMutableArray *reportData = [[self getReportErrorcodeData:interfaceName withinterfaceDesc:interfaceDesc withErrorCode:errorCode withErrorDesc:errorDesc withClientNumber:clientNumber] retain];
    



 
    
    if ([NSJSONSerialization isValidJSONObject:reportData])
    {
        
        NSError *error;
        NSData *jsonData = [NSJSONSerialization dataWithJSONObject:reportData options:NSJSONWritingPrettyPrinted error: &error];
        NSMutableData *tempJsonData = [NSMutableData dataWithData:jsonData];
        
        
        NSString *jsonString =[[NSString alloc] initWithData:tempJsonData encoding:NSUTF8StringEncoding];
        NSLog(@"Register JSON:%@",jsonString);
        
        
        NSURL *url =  nil;
        
        if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"TESTMODE"])//测试
        {
             url = [NSURL URLWithString:[NSString stringWithFormat:@"http://113.31.89.144:8088/ulog/log?event=errorCode"]];
        }
        else if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"DEVMODE"])//DEV 模式
        {
            //开发环境
//            url = [NSURL URLWithString:[NSString stringWithFormat:@"http://113.31.89.144:8088/ulog/log?event=errorCode"]];
        }
        else //正式
        {
 
             url = [NSURL URLWithString:[NSString stringWithFormat:@"http://ulog.ucpaas.com/ulog/log?event=errorCode"]];
        }
        
        


        
        UCSASIHTTPRequest *request = [UCSASIHTTPRequest requestWithURL:url];
        
        //    [request setValidatesSecureCertificate:NO];//请求为HTTPS时需要设置这个属性
        
        [request addRequestHeader:@"Content-Type" value:@"application/json; encoding=utf-8"];
        [request addRequestHeader:@"Accept" value:@"application/json"];
        //    [request addRequestHeader:@"Authorization" value:auth_base64];
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
        NSLog(@"----------------UCSStatiRequestManager-------------requestFinished:%@",returnDic);

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
