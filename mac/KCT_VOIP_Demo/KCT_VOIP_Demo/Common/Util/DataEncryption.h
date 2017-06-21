//
//  DataEncryption.h
//  KCT_VOIP_Demo
//
//  Created by KCMac on 2017/1/7.
//  Copyright © 2017年 KCMac. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface DataEncryption : NSObject

+(NSString *)encodeBase64String:(NSString *)str;
+(NSString *)md5CapitalizedString:(NSString *)str;
+(NSString *)md5Data:(NSData *)data;

@end
