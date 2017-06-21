//
//  KCTTCPCommonClass.m
//  Tcplib
//
//  Created by KCT on 16/4/11.
//  Copyright © 2017年 KCTMAC. All rights reserved.
//

#import "KCTTCPCommonClass.h"
#import <objc/runtime.h>
#import "KCTProtocolClass.h"

@implementation KCTTCPTransParentRequest : NSObject

+(instancetype)initWithCmdString:(NSString *)cmdString receiveId:(NSString *)receiveId{
     return  [[self alloc] initWithCmdString:cmdString receiveId:receiveId];
}

-(instancetype)initWithCmdString:(NSString*)cmdString
                  receiveId:(NSString *)receiveId{
    self = [super init];
    if (self) {
        _receiveId = [receiveId copy];
        _cmdString = [cmdString copy];
    }
    return  self;
}

UCSDescription

@end



@interface KCTTCPTransParent ()
@property (nonatomic, strong) NSNumber *serverId;

@end

@implementation KCTTCPTransParent


UCSDescription
@end
