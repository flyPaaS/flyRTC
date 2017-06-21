//
//  KCTVoipCallListModel.m
//  KCT_IM_Demo
//
//  Created by macmini on 15/12/15.
//  Copyright © 2015年 KCMac. All rights reserved.
//

#import "KCTVoipCallListModel.h"

@implementation KCTVoipCallListModel

- (void)dealloc{
    self.userId = nil;
    self.nickName = nil;
    self.time = nil;
    self.callDuration = nil;
    self.headPortrait = nil;
    self.callType = nil;
    self.callStatus = nil;
    self.sendCall = nil;
}

- (instancetype)init{
    
    if (self = [super init]) {
    }

    return self;
}


- (BOOL)checkModelInfo{
    
    if (self.userId == nil ||
        self.nickName == nil ||
        self.time == nil ||
        self.callDuration == nil||
        self.headPortrait == nil ||
        self.callType == nil ||
        self.callStatus ==nil ||
        self.sendCall == nil) {
        
        
        return YES;
    }
    
    return NO;
}




@end
