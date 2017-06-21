//
//  KCSCatchExceptionHandler.h
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/7/6.
//  Copyright (c) 2015年 Barry. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface UCSVoipCatchExceptionHandler : NSObject

/**
 *  初始化编译时间
 */
+ (void)setCompileTime;

+ (void)setDefaultHandler;

+ (NSUncaughtExceptionHandler *)getHandler;

+(NSString *)getCurrentSysTime;
+(NSString *)currentCompileTime;

@end
