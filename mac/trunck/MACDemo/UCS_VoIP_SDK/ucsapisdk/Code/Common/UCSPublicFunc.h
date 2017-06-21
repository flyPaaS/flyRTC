//
//  UCSPublicFunc.h
//  yzxapisdk
//
//  Created by wenqinglin on 14-4-9.
//  Copyright (c) 2014年 yzx. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface UCSPublicFunc : NSObject

// 获取Http服务器地址
+ (NSString *) GetHttpServer;

+ (PhoneNetType) GetCurrentPhoneNetType;

+(NSDictionary*)checkNetworkflow;

+(long long) fileSizeAtPath:(NSString*) filePath;

+(NSString *)getIsUseTestServer;

+(NSString *)getIPhoneNowTime;


@end
