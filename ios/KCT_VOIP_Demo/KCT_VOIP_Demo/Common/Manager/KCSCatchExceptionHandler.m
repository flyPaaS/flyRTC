//
//  KCSCatchExceptionHandler.m
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/7/6.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import "KCSCatchExceptionHandler.h"

#define NSExceptionFile @"Demo errorFile.txt"
#define NSCompileTime @"NSCompileTime"

@implementation KCSCatchExceptionHandler

+ (void)setCompileTime{
    NSDate *  senddate=[NSDate date];
    NSDateFormatter  *dateformatter=[[NSDateFormatter alloc] init];
    [dateformatter setDateFormat:@"yyyyMMdd hh:mm"];
    NSString *  locationString=[dateformatter stringFromDate:senddate];
    [[NSUserDefaults standardUserDefaults] setValue:locationString forKey:NSCompileTime];
    [[NSUserDefaults standardUserDefaults] synchronize];
}

NSString *applicationDocumentsDirectory() {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask,YES);
    NSString * documentsDirectory  =[paths objectAtIndex:0];
    return [documentsDirectory stringByAppendingPathComponent:NSExceptionFile];
}

void UncaughtExceptionHandler(NSException *exception)
{
    NSArray *arr = [exception callStackSymbols];
    NSString *reason = [exception reason];
    NSString *name = [exception name];
    NSString *strTime =[KCSCatchExceptionHandler getCurrentSysTime];
    
    NSString *strError = [NSString stringWithFormat:@"\n\n\n=============异常崩溃报告=============\n当前版本的编译时间:\n%@\n崩溃发生的时间:\n %@\n崩溃名称:\n%@\n崩溃原因:\n%@\n堆栈信息:\n%@", [KCSCatchExceptionHandler currentCompileTime],strTime,name,reason,[arr componentsJoinedByString:@"\n"]];
    
    NSString *path = applicationDocumentsDirectory();
    if ([[NSFileManager defaultManager]fileExistsAtPath:path])
    {
        NSString *lasterror = [[NSString alloc] initWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
        strError  = [NSString stringWithFormat:@"%@%@", lasterror, strError];
    }
    [strError writeToFile:path atomically:YES encoding:NSUTF8StringEncoding error:nil];
    
}


+ (void)setDefaultHandler
{
    NSSetUncaughtExceptionHandler (&UncaughtExceptionHandler);
}

+ (NSUncaughtExceptionHandler*)getHandler
{
    return NSGetUncaughtExceptionHandler();
}


+(NSString *)getCurrentSysTime{
    NSDate *  senddate=[NSDate date];
    NSDateFormatter  *dateformatter=[[NSDateFormatter alloc] init];
    [dateformatter setDateFormat:@"yyyyMMdd hh:mm"];
    return [dateformatter stringFromDate:senddate];
}

+ (NSString *)currentCompileTime{
    return [[NSUserDefaults standardUserDefaults] stringForKey:NSCompileTime];
}

@end
