//
//  KCSCatchExceptionHandler.m
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/7/6.
//  Copyright (c) 2015年 Barry. All rights reserved.
//

#import "UCSVoipCatchExceptionHandler.h"

#define NSExceptionFile @"UCSCrashLog.txt"
#define NSCompileTime @"NSCompileTime"

@implementation UCSVoipCatchExceptionHandler

+ (void)setCompileTime{
    NSDate *  senddate=[NSDate date];
    NSDateFormatter  *dateformatter=[[NSDateFormatter alloc] init];
    [dateformatter setDateFormat:@"yyyyMMddHHmmss"];
    NSString *  locationString=[dateformatter stringFromDate:senddate];
    [[NSUserDefaults standardUserDefaults] setValue:locationString forKey:NSCompileTime];
    [[NSUserDefaults standardUserDefaults] synchronize];
    [dateformatter release];
}

NSString *VoipApplicationDocumentsDirectory() {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask,YES);
    NSString * documentsDirectory  =[paths objectAtIndex:0];
    return [documentsDirectory stringByAppendingPathComponent:NSExceptionFile];
}

void UCSVoipUncaughtExceptionHandler(NSException *exception)
{
    NSArray *arr = [exception callStackSymbols];
    NSString *reason = [exception reason];
    NSString *name = [exception name];
    NSString *strTime =[UCSVoipCatchExceptionHandler getCurrentSysTime];
    
    // 通话类型
    NSString * callType = [UserDefaultManager GetLocalDataString:DATA_STORE_SERVICEKEY];
    if (callType.length == 0) {
        callType = @"VOIP";
    }
    NSMutableString * errorStr = [NSMutableString stringWithFormat:@"LOG:\t%@\t%@\t%@\t%@\t",strTime,callType,@"SDK",@"IOS"];

    
    NSString *strError = [NSString stringWithFormat:@"%@\"\n\n\n=============异常崩溃报告=============\n当前版本的编译时间:\n%@\n崩溃发生的时间:\n %@\n崩溃名称:\n%@\n崩溃原因:\n%@\n堆栈信息:\n%@\"",errorStr ,[UCSVoipCatchExceptionHandler currentCompileTime],strTime,name,reason,[arr componentsJoinedByString:@"\n"]];
    
    NSString *path = VoipApplicationDocumentsDirectory();
    NSFileManager * fileManager = [NSFileManager defaultManager];
    if ([[NSFileManager defaultManager] fileExistsAtPath:path])
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
        
        [outFile writeData:[strError dataUsingEncoding:NSUTF8StringEncoding]];
        //关闭读写文件
        [outFile closeFile];
    }else{
        // 如果文件不存在 则创建并且将文件写入
        [fileManager createFileAtPath:path contents:nil attributes:nil];
        [strError writeToFile:path atomically:YES encoding:NSUTF8StringEncoding error:nil];
    }
    // 获取文件名，如果不存在则重新创建
    NSString * logName = [[NSUserDefaults standardUserDefaults] valueForKey:@"UCSLogName"];
    if (logName.length == 0 || !logName) {
        logName = [NSString stringWithFormat:@"crash_%@_%@_%@.txt",[UserDefaultManager GetKeyChain:DATA_STORE_APPID],[UserDefaultManager GetclientNumber],strTime];
        [[NSUserDefaults standardUserDefaults] setValue:logName forKey:@"UCSLogName"];
        [[NSUserDefaults standardUserDefaults] synchronize];
    }
    // 需要上传的文件 写入
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask,YES);
    NSString * documentsDirectory  =[paths objectAtIndex:0];
    NSString * filePath = [documentsDirectory stringByAppendingPathComponent:logName];
    if ([fileManager fileExistsAtPath:filePath]) {
        NSFileHandle * outFile = [NSFileHandle fileHandleForWritingAtPath:filePath];
        [outFile seekToEndOfFile];
        [outFile writeData:[strError dataUsingEncoding:NSUTF8StringEncoding]];
        [outFile closeFile];
    }else{
        // 如果文件不存在 则创建并且将文件写入
        [fileManager createFileAtPath:filePath contents:nil attributes:nil];
        [strError writeToFile:filePath atomically:YES encoding:NSUTF8StringEncoding error:nil];
    }
}


+ (void)setDefaultHandler
{
    NSSetUncaughtExceptionHandler (&UCSVoipUncaughtExceptionHandler);
}

+ (NSUncaughtExceptionHandler*)getHandler
{
    return NSGetUncaughtExceptionHandler();
}


+(NSString *)getCurrentSysTime{
    NSDate *  senddate=[NSDate date];
    NSDateFormatter  *dateformatter=[[[NSDateFormatter alloc] init]autorelease];
    [dateformatter setDateFormat:@"yyyyMMddHHmmss"];
    return [dateformatter stringFromDate:senddate];
}

+ (NSString *)currentCompileTime{
    return [[NSUserDefaults standardUserDefaults] stringForKey:NSCompileTime];
}

@end
