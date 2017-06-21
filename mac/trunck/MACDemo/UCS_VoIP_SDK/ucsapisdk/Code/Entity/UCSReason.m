//
//  KCTReason.m
//  KCTApiSdk
//
//  Created by wenqinglin on 14-5-20.
//  Copyright (c) 2014年 ucs. All rights reserved.
//

#import "KCTCommonClass.h"

@implementation KCTReason

@synthesize reason = _reason;
@synthesize msg = _msg;
@synthesize callId = _callId;

-(void)dealloc
{
    [_callId release];
    [_msg release];
    [super dealloc];
}

- (id)copyWithZone:(NSZone *)zone
{
    
    KCTReason *copy = [[KCTReason allocWithZone:zone] init];
    
    copy.reason = self.reason;
    copy.msg = self.msg;
    copy.callId = self.callId;
    
    return  copy;
    
}


@end
