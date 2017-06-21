//
//  SDKLogManager.m
//  ucsvoicesdk
//
//  Created by FredZhang on 15-1-29.
//  Copyright (c) 2015年 ucs. All rights reserved.
//

#import "SDKLogManager.h"
#import "UCSExceptionHandler.h"
#import "UCSVoipCatchExceptionHandler.h"
SDKLogManager *g_sdkLogManager = nil;


#import "UCSVoipCommonFunc.h"


@implementation SDKLogManager

+ (SDKLogManager *) instance
{
    if (nil == g_sdkLogManager)
    {
        g_sdkLogManager = [[self alloc] init];
    }
    
    return g_sdkLogManager;
}

- (id) init
{
    self = [super init];
    if (self)
    {
        
    }
    
    return self;
}

- (void) saveSDKLogInfo:(NSString *) summary withDetail:(NSString *) detail
{
    NSLog(@"%@ -- %@", summary, detail);
    
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
    
    [self sdklogc_Write:strSummary andStrDetail:strDetail];
    
    [strSummary release];
    [strDetail release];
}


/******/
//将日志写入指定文件
-(void) sdklogc_Write:(NSString*)strSummary andStrDetail:(NSString*)strDetail
{
    
    NSDate *yesterday = [NSDate dateWithTimeIntervalSince1970:[[NSDate date] timeIntervalSince1970]];
    NSDateFormatter  *formatter = [[NSDateFormatter alloc] init];
    [formatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    NSString *timeString = [NSMutableString stringWithFormat:@"%@",[formatter stringFromDate:yesterday]];
    [formatter release];
    
    NSString *url = [NSString stringWithFormat:@"\n\n=============SDK 日志=============时间:%@\nSummary:\n%@\n\nstrDetail:\n%@\n\n堆栈:%@\n\n",timeString,strSummary,strDetail,[UCSVoipCommonFunc ucsVoipGetcallstack:30]];
    NSString *path = sdk_applicationDocumentsDirectory();// stringByAppendingPathComponent:@"Exception.txt"];
    
//    UXLogAlert(@"%@",url);
    
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
NSString *sdk_applicationDocumentsDirectory()
{
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    documentsDirectory = [documentsDirectory stringByAppendingPathComponent:@"SDKLog"];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if (![fileManager fileExistsAtPath:documentsDirectory])
    {
        NSError *err = nil;
        [fileManager createDirectoryAtPath:documentsDirectory withIntermediateDirectories:YES attributes:nil error:&err];
        
        NSString *filePathMe = [documentsDirectory stringByAppendingPathComponent:@"ios_sdk_log.txt"];
        //查找文件，如果不存在，就创建一个文件
        if (![fileManager fileExistsAtPath:filePathMe])
        {
            [fileManager createFileAtPath:filePathMe contents:nil attributes:nil];
            
        }
        else
        {
            //如果文件存在并且它的大小大于1M，则删除并且重新创建一个
            long long filesizes  = [[fileManager attributesOfItemAtPath:filePathMe error:nil] fileSize];
            if ((filesizes/(1024.0*1024.0))>1) {
                
                //删除当前文件
                [fileManager removeItemAtPath:filePathMe error:nil];
                //重新创建一个文件
                [fileManager createFileAtPath:filePathMe contents:nil attributes:nil];
            }
            
        }
        
        if (err) {
            
            return nil;
        }
        else
        {
            return [documentsDirectory stringByAppendingPathComponent:@"ios_sdk_log.txt"];
        }
    }
    else
    {
        return [documentsDirectory stringByAppendingPathComponent:@"ios_sdk_log.txt"];
    }
    
    return [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject];
}

#pragma mark 异常日志

- (void)setSDKDefaultHandler
{
//    NSSetUncaughtExceptionHandler (&sdk_UncaughtExceptionHandler);
    // 保存错误日志  CW 2016.1.13
    [UCSVoipCatchExceptionHandler setCompileTime];
    [UCSVoipCatchExceptionHandler setDefaultHandler];
//    UCSInstallUncaughtExceptionHandler(); //异常捕获 WLS 20150727

}
void sdk_UncaughtExceptionHandler(NSException *exception)
{
    NSArray *arr = [exception callStackSymbols];
    NSString *reason = [exception reason];
    NSString *name = [exception name];
    
    [[SDKLogManager instance]saveSDKLogInfo:@"异常崩溃" withDetail:[NSString stringWithFormat:@"\n\n=============异常崩溃报告=============\nname:\n%@\nreason:\n%@\ncallStackSymbols:\n%@",name,reason,[arr componentsJoinedByString:@"\n"]]];
    
}

+ (void)saveConversationLogWithDetail:(NSString *)detail{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask,YES);
    NSString * documentsDirectory  =[paths objectAtIndex:0];
    NSString * path = [documentsDirectory stringByAppendingPathComponent:@"voipConversationLog.txt"];
    NSFileManager * fileManager = [NSFileManager defaultManager];
    if ([fileManager fileExistsAtPath:path])
    {
        //如果文件存在并且它的大小大于3M，则删除并且重新创建一个
        long long filesizes  = [[fileManager attributesOfItemAtPath:path error:nil] fileSize];
        if ((filesizes/(1024.0*1024.0))>3) {
            
            //删除当前文件
            [fileManager removeItemAtPath:path error:nil];
            //重新创建一个文件
            [fileManager createFileAtPath:path contents:nil attributes:nil];
        }
        
        NSFileHandle *outFile = [NSFileHandle fileHandleForWritingAtPath:path];
        //找到并定位到outFile的末尾位置(在此后追加文件)
        [outFile seekToEndOfFile];
        
        [outFile writeData:[detail dataUsingEncoding:NSUTF8StringEncoding]];
        //关闭读写文件
        [outFile closeFile];
    }else{
        // 如果文件不存在 则创建并且将文件写入
        [fileManager createFileAtPath:path contents:nil attributes:nil];
        [detail writeToFile:path atomically:YES encoding:NSUTF8StringEncoding error:nil];
    }
}


/**
 @author WLS, 16-02-01 14:02:04
 
 根据文件名字创建文件
 
*/
+ (NSString *)getLogFileWithName:(NSString *)name{
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    documentsDirectory = [documentsDirectory stringByAppendingPathComponent:name];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    if (![fileManager fileExistsAtPath:documentsDirectory])
    {
        NSError *err = nil;
        [fileManager createDirectoryAtPath:documentsDirectory withIntermediateDirectories:YES attributes:nil error:&err];
        
        NSString *filePathMe = [documentsDirectory stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.txt",name]];
        //查找文件，如果不存在，就创建一个文件
        if (![fileManager fileExistsAtPath:filePathMe])
        {
            [fileManager createFileAtPath:filePathMe contents:nil attributes:nil];
            
        }else{
            //如果文件存在并且它的大小大于10M，则删除并且重新创建一个
            long long filesizes  = [[fileManager attributesOfItemAtPath:filePathMe error:nil] fileSize];
            if ((filesizes/(1024.0*1024.0))>10) {
                
                //删除当前文件
                [fileManager removeItemAtPath:filePathMe error:nil];
                //重新创建一个文件
                [fileManager createFileAtPath:filePathMe contents:nil attributes:nil];
            }
            
        }
        
        if (err) {
            
            return @"";
        } else   {
            return [documentsDirectory stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.txt",name]];
        }
    }  else {
        return [documentsDirectory stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.txt",name]];
    }
    
    return [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) lastObject];
    
}


@end
