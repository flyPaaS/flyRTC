//
//  base64.h
//  UXinClient
//
//  Created by Liam Peng on 11-9-29.
//  Copyright 2011年 UXIN. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface uxinbase64 : NSObject 
{
}

+ (NSString * )encodeBase64:(NSString * )input;
+ (NSString * )decodeBase64:(NSString * )input;

@end