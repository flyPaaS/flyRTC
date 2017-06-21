//
//  UCSTCPRealConnecter.m
//  Tcplib
//
//  Created by KCT on 16/3/31.
//  Copyright © 2017年 KCTMAC. All rights reserved.
//

#import "UCSTCPRealConnecter.h"

@implementation UCSTCPRealConnecter

static id _instace;

+(instancetype)sharedInstance
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instace = [[self alloc] init];
    });
    return _instace;
}
-(id)init{
    self = [super init];
    if (self) {
        _realConnected = NO;
    }
    
    return self;
}

@end
