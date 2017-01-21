//
//  KCTCallRecordsModel.h
//  KCT_IM_Demo
//
//  Created by macmini on 15/12/15.
//  Copyright © 2015年 KCT. All rights reserved.
//

#import <Foundation/Foundation.h>

@class KCTVoipCallListModel;
@interface KCTCallRecordsModel : NSObject


@property (copy, nonatomic) NSString * userId;       // 账号ID
@property (copy, nonatomic) NSString * nickName;     // 账号昵称
@property (copy, nonatomic) NSString * time;         // 拨打或者接听时间
@property (copy, nonatomic) NSString * callDuration; // 通话时间
@property (copy, nonatomic) NSString * headPortrait; // 账号头像
@property (copy, nonatomic) NSString * callType;     // 通话类型 (语音通话，单向外呼，视频通话)
@property (copy, nonatomic) NSString * callStatus;   // 通话状态 （接听0，已取消1，未接听2）
@property (copy, nonatomic) NSString * sendCall;     // 来电还是拨打

- (void)getInfoFromCallListModel:(KCTVoipCallListModel *)model;

@end
