//
//  LocationNotificationManager.h
//  UC_Demo_1.0.0
//
//  Created by Barry on 15/5/28.
//  Copyright (c) 2015年 Barry. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "UCSMessage.h"



@interface LocationNotificationManager : NSObject


@property (nonatomic, copy) NSString * userName; //消息的发送者,单聊或者群聊成员
@property (nonatomic, copy) NSString * content; //消息体
@property (nonatomic, copy) NSString * chatter; // 讨论组ID,群组id,单聊对方id
@property (nonatomic, copy) NSString * discussionName; //讨论组名
@property (nonatomic, copy) NSString * groupName; //群组名
@property(nonatomic, assign) UCS_IM_ConversationType conversationType;

- (instancetype)initWithMessage:(UCSMessage *) message;

@end
