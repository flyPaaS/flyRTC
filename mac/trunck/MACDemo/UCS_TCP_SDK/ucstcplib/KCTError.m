//
//  KCTError.m
//  Tcplib
//
//  Created by wenqinglin on 15/8/27.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import "KCTError.h"
#import <objc/runtime.h>

@implementation KCTError

+(instancetype)errorWithCode:(KCTErrorCode)errorCode andDescription:(NSString *)errorDescription{
    
     return  [[self alloc] initWithCode:errorCode andDescription:errorDescription];
}

- (instancetype)initWithCode:(KCTErrorCode) errorCode andDescription:(NSString *)errorDescription{
    
    self = [super init];
    if (self) {
        _code = errorCode;
        _errorDescription = [errorDescription copy];
    }
    return self;
}




- (NSString *)description
{
    NSString *result = @"";
    NSArray *proNames = [self getAllProperties];
    for (int i = 0; i < proNames.count; i++) {
        NSString *proName = [proNames objectAtIndex:i];
        id  proValue = [self valueForKey:proName];
        result = [result stringByAppendingFormat:@"%@:%@\n",proName,proValue];
    }
    return result;
}

- (NSArray *)getAllProperties
{
    u_int count;
    objc_property_t *properties  =class_copyPropertyList([self class], &count);
    NSMutableArray *propertiesArray = [NSMutableArray arrayWithCapacity:count];
    for (int i = 0; i<count; i++)
    {
        const char* propertyName =property_getName(properties[i]);
        [propertiesArray addObject: [NSString stringWithUTF8String: propertyName]];
    }
    free(properties);
    return propertiesArray;
}

@end
