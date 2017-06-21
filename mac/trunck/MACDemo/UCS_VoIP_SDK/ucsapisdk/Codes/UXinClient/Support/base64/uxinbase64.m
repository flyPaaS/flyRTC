//
//  uxinbase64.m
//  UXinClient
//
//  Created by Liam Peng on 11-9-29.
//  Copyright 2011年 UXIN. All rights reserved.
//

#import "uxinbase64.h"



#import "UxinGTMBase64.h"

@implementation uxinbase64


+(NSString *) encodeBase64:(NSString *)input 
{ 
    NSData * data = [input dataUsingEncoding:NSUTF8StringEncoding allowLossyConversion:YES]; 
    
    // 转换到base64 
    data = [UxinGTMBase64 encodeData:data]; 
    NSString * base64String = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding]; 
    
    return [base64String autorelease]; 
}

+(NSString *) decodeBase64:(NSString *)input 
{ 
    NSData * data = [input dataUsingEncoding:NSUTF8StringEncoding allowLossyConversion:YES]; 
    
    // 转换到base64 
    data = [UxinGTMBase64 decodeData:data]; 
    NSString * base64String = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding]; 
    
    return [base64String autorelease]; 
}

@end
