//
//  NSDictionary+JsonBase64.h
//  KCT_IM_Demo
//
//  Created by macmini on 15/12/16.
//  Copyright © 2015年 KCMac. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSDictionary (JsonBase64)

+ (NSString *)jsonbase64WithDictionary:(NSDictionary *)dic;

@end
