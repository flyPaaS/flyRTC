//
//  KCTCallRecordsModel.m
//  KCT_IM_Demo
//
//  Created by macmini on 15/12/15.
//  Copyright © 2015年 KCT. All rights reserved.
//

#import "KCTCallRecordsModel.h"
#import "KCTVoipCallListModel.h"
@implementation KCTCallRecordsModel


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

- (void)getInfoFromCallListModel:(KCTVoipCallListModel *)model{
    
    self.userId = model.userId;
    self.nickName = model.nickName;
    self.time = model.time;
    self.callDuration = model.callDuration;
    self.headPortrait = model.headPortrait;
    self.callType = model.callType;
    self.callStatus = model.callStatus;
    self.sendCall = model.sendCall;
    
    
}

@end
