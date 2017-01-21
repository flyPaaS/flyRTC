//
//  NSDictionary+JsonBase64.m
//  KCT_IM_Demo
//
//  Created by macmini on 15/12/16.
//  Copyright © 2015年 KCT. All rights reserved.
//

#import "NSDictionary+JsonBase64.h"
#import "GTMBase64.h"

@implementation NSDictionary (JsonBase64)

+ (NSString *)jsonbase64WithDictionary:(NSDictionary *)dic{
    NSError * error = nil;
    // 转成json的二进制
    NSData * jsonData = [NSJSONSerialization dataWithJSONObject:dic options:NSJSONWritingPrettyPrinted error:&error];
    // 转成json的字符串
    NSString * jsonStr = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    // 去掉回车和空格
    NSString * str = [[jsonStr stringByReplacingOccurrencesOfString:@"\n" withString:@""] stringByReplacingOccurrencesOfString:@" " withString:@""];
    // base64 加密
    NSString * base64Str = [GTMBase64 encodeBase64String:str];

    return base64Str;
}


@end
