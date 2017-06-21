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
#import "UCSVoipZipArchive.h"

// 电话日志上传相关
// 获取电话日志上传配置参数
#define URL_CALL_LOG_CONFIG                 @"http://omp.guoling.com/reportcontrol.action?bid=%@&pv=%@&v=%@&sipv=%@"
// 上传电话日志
//#if defined (IsTestMode)
 #define URL_CALL_LOG_COMMIT_TEST            @"http://113.31.89.144:8088/ulog/log?event=logfile&uid=%@"
//#else
 #define URL_CALL_LOG_COMMIT                 @"http://ulog.ucpaas.com/ulog/log?event=logfile&uid=%@"
//#endif

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
//    NSLog(@"%@ -- %@", summary, detail);
    
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
    
    NSString *strSRC = [NSString stringWithFormat:@"%@_%@_%@_%@_%@", @"UCPaas", [NSString stringWithFormat:@"%@%@", @"macName", @"1.0"], kPhoneType, uVersion, @"UCPaas"];
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
    
    [self logcallback_Write:strSummary andStrDetail:strDetail];
//    NSMutableDictionary *dicListItem = [NSMutableDictionary dictionary];
//    [dicListItem setObject:strSRC forKey:@"src"];
//    [dicListItem setObject:strCategory forKey:@"category"];
//    [dicListItem setObject:strSummary forKey:@"summary"];
//    [dicListItem setObject:strDetail forKey:@"detail"];
//    
//    NSMutableDictionary *dicContent = [NSMutableDictionary dictionary];
//    [dicContent setObject:@"UCPaas" forKey:@"bid"];
//    [dicContent setObject:[UserDefaultManager GetUserID] forKey:@"uid"];
//    [dicContent setObject:[self getPV] forKey:@"pv"];
//    [dicContent setObject:uVersion forKey:@"v"];
//    [dicContent setObject:[UserDefaultManager GetUserMobile] forKey:@"call"];
//    [dicContent setObject:self.strCalled forKey:@"called"];
//    
//    NSMutableArray *arrayListInfo = [self.dicUploadInfo objectForKey:@"list"];
//    if (nil != arrayListInfo)
//    {
//        [arrayListInfo addObject:dicListItem];
//    }
//    else
//    {
//        arrayListInfo = [NSMutableArray array];
//        [arrayListInfo addObject:dicListItem];
//    }
//    [self.dicUploadInfo setObject:arrayListInfo forKey:@"list"];
//    [self.dicUploadInfo setObject:dicContent forKey:@"content"];
//    
    [strSummary release];
    [strDetail release];
}


/******/
//将日志写入指定文件
-(void) logcallback_Write:(NSString*)strSummary andStrDetail:(NSString*)strDetail
{
 
    
    NSDate *yesterday = [NSDate dateWithTimeIntervalSince1970:[[NSDate date] timeIntervalSince1970]];
    NSDateFormatter  *formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    NSString *timeString = [NSMutableString stringWithFormat:@"%@",[formatter stringFromDate:yesterday]];
    [formatter release];
    
    NSString *url = [NSString stringWithFormat:@"\n\n=============logcallback_func回调日志上报=============时间:%@\nSummary:\n%@\n\nstrDetail:\n%@\n\n",timeString,strSummary,strDetail];
    NSString *path = applicationDocumentsDirectory();// stringByAppendingPathComponent:@"Exception.txt"];
    
//    NSLog(@"%@",url);
    
    //一次性读写
//    [url writeToFile:path atomically:YES encoding:(NSUTF8StringEncoding) error:nil];
    
    
    NSFileHandle *outFile = [NSFileHandle fileHandleForWritingAtPath:path];
    //找到并定位到outFile的末尾位置(在此后追加文件)
    [outFile seekToEndOfFile];
  
    [outFile writeData:[url dataUsingEncoding:NSUTF8StringEncoding]];
    //关闭读写文件
    [outFile closeFile];
 
    
}
//日志存放路径
NSString *applicationDocumentsDirectory()
{
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    documentsDirectory = [documentsDirectory stringByAppendingPathComponent:@"logcallback"];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if (![fileManager fileExistsAtPath:documentsDirectory])
    {
        NSError *err = nil;
        [fileManager createDirectoryAtPath:documentsDirectory withIntermediateDirectories:YES attributes:nil error:&err];
        
        NSString *filePathMe = [documentsDirectory stringByAppendingPathComponent:@"logcallback_func.txt"];
        //查找文件，如果不存在，就创建一个文件
        if (![fileManager fileExistsAtPath:filePathMe])
        {
            [fileManager createFileAtPath:filePathMe contents:nil attributes:nil];

        } else {
            //如果文件存在并且它的大小大于1M，则删除并且重新创建一个
            long long filesizes  = [[fileManager attributesOfItemAtPath:filePathMe error:nil] fileSize];
            if ((filesizes/(1024.0*1024.0))>10) {
                
                //删除当前文件
                [fileManager removeItemAtPath:filePathMe error:nil];
                //重新创建一个文件
                [fileManager createFileAtPath:filePathMe contents:nil attributes:nil];
            }
        }
        
        
        
        if (err) {
            
            return nil;
            
        } else  {
            return [documentsDirectory stringByAppendingPathComponent:@"logcallback_func.txt"];
        }
    } else {
        
        [fileManager createDirectoryAtPath:documentsDirectory withIntermediateDirectories:YES attributes:nil error:nil];
        
        NSString *filePathMe = [documentsDirectory stringByAppendingPathComponent:@"logcallback_func.txt"];
        //查找文件，如果不存在，就创建一个文件
        if (![fileManager fileExistsAtPath:filePathMe])
        {
            [fileManager createFileAtPath:filePathMe contents:nil attributes:nil];
            
        } else {
            //如果文件存在并且它的大小大于1M，则删除并且重新创建一个
            long long filesizes  = [[fileManager attributesOfItemAtPath:filePathMe error:nil] fileSize];
            if ((filesizes/(1024.0*1024.0))>1) {
                
                //删除当前文件
                [fileManager removeItemAtPath:filePathMe error:nil];
                //重新创建一个文件
                [fileManager createFileAtPath:filePathMe contents:nil attributes:nil];
            }
        }
        
        return [documentsDirectory stringByAppendingPathComponent:@"logcallback_func.txt"];
    }
    
    return [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject];
}

//将传入文件打成一个压缩包,并且返压缩包路径
-(NSString*)ZiplogcallbackFile:(NSString*) logFilePath{
    
    
    UCSVoipZipArchive* zip = [[UCSVoipZipArchive alloc] init];
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *dcoumentpath = ([paths count] > 0) ? [paths objectAtIndex:0] : nil;
    
    dcoumentpath = [dcoumentpath stringByAppendingPathComponent:@"logcallback"];
    
    NSString * tempzipname = [NSString stringWithFormat:@"/%@.zip",[UserDefaultManager GetUserID]];
    
    //文件管理句柄
    NSFileManager *fileManager = [NSFileManager defaultManager];
    
    //压缩成功后的文件
 
    NSString* l_zipfile = [dcoumentpath stringByAppendingString:tempzipname];
    


    //如果存在则先删除这个压缩包
    if ([fileManager fileExistsAtPath:l_zipfile])
    {
       [fileManager removeItemAtPath:l_zipfile error:nil];
    }
    
    
    NSString* addfile1 = logFilePath;//放入压缩包中的文件
 
    
    BOOL ret = [zip CreateZipFile2:l_zipfile];
    
    ret = [zip addFileToZip:addfile1 newname:@"logcallback_func_zip.txt"];

    if( ![zip CloseZipFile2] )
    {
        l_zipfile = @"";
    }
    [zip release];
    
    
    return l_zipfile;

}
//上传成功后删除本地文件
-(BOOL)ZipFileUploadSuccAfterRemove:(NSString*) logFilePath{

            NSFileManager *fileManager = [NSFileManager defaultManager];
    
           BOOL result = [fileManager removeItemAtPath:logFilePath error:nil];
    
    return result;
}

/*******/






- (void) submitLogToServer
{
    [self performSelector:@selector(realSubmit) withObject:nil afterDelay:1.0f];
}


- (void) realSubmit
{
      NSString *IslogReport =  [UserDefaultManager  GetKeyChain:DATA_STORE_CPS_LOGREPORT];
    if ([IslogReport isEqualToString:@"1"]) {
        
        
        NSString * url_call_log_commit = nil;
        
             if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"TESTMODE"])//测试
            {
                url_call_log_commit = URL_CALL_LOG_COMMIT_TEST;
            }
            else if ([UCSPublicFunc getIsUseTestServer] && [[UCSPublicFunc getIsUseTestServer] isEqualToString:@"DEVMODE"])//DEV 模式
            {
//                url_call_log_commit = URL_CALL_LOG_COMMIT_TEST;
            }
            else //正式
            {
                url_call_log_commit = URL_CALL_LOG_COMMIT;
            }
        
        
      
 
            UCSASIFormDataRequest *pUCSASIHTTPRequest = [UCSASIFormDataRequest requestWithURL:[NSURL URLWithString:[NSString stringWithFormat:url_call_log_commit,[UserDefaultManager GetUserID] ]]];
 
            [pUCSASIHTTPRequest setTimeOutSeconds:360.0f];
            //获取日志路径，传给压缩函数进行压缩
            NSString * strFileName =  [self ZiplogcallbackFile:applicationDocumentsDirectory()];
            [pUCSASIHTTPRequest addFile:strFileName forKey:@"only one value"];
    
    
            // 设定委托，委托自己实现异步请求方法
            [pUCSASIHTTPRequest setDelegate:self];
            [pUCSASIHTTPRequest startAsynchronous];
 
    }

    
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
    [returnDic setDictionary:[NSJSONSerialization JSONObjectWithData:responseData options:0 error:nil]];
    
    //上传成功后删除日志文件
//    BOOL remresult = [self ZipFileUploadSuccAfterRemove: applicationDocumentsDirectory()];
    //上传成功后删除打成压缩包日志文件
    BOOL remzipresult = [self ZipFileUploadSuccAfterRemove:[self ZiplogcallbackFile:applicationDocumentsDirectory()]];
    
    //上报成功后关闭上报开关
    [UserDefaultManager SetKeyChain:@"0" key:DATA_STORE_CPS_LOGREPORT];
    
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
    NSMutableDictionary *returnDic = [NSMutableDictionary dictionary];
    [returnDic setDictionary:[NSJSONSerialization JSONObjectWithData:data options:0 error:nil]];
    
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
