//
//  SDKLogManager.h
//  ucsvoicesdk
//
//  Created by FredZhang on 15-1-29.
//  Copyright (c) 2015年 ucs. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SDKLogManager : NSObject

+ (SDKLogManager *) instance;

- (void)setSDKDefaultHandler;

- (void) saveSDKLogInfo:(NSString *) summary withDetail:(NSString *) detail;

+ (void) saveConversationLogWithDetail:(NSString *)detail;


+ (NSString *)getLogFileWithName:(NSString *)name;

@end
