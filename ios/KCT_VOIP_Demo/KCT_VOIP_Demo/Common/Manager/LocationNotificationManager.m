//
//  LocationNotificationManager.m
//  UC_Demo_1.0.0
//
//  Created by Barry on 15/5/28.
//  Copyright (c) 2015年 Barry. All rights reserved.
//

#import "LocationNotificationManager.h"
#import "UCSTextMsg.h"
#import "UCSVoiceMsg.h"
#import "UCSImageMsg.h"
#import "UCSMsgContent.h"
#import "UCSDiscussionNotification.h"
#import "NSString+Emojize.h"

@implementation LocationNotificationManager

- (instancetype)initWithMessage:(UCSMessage *)message{
    self = [super init];
    if (self) {
        
        UCSMsgContent * messageContent = message.content;
        if (message.messageType == UCS_IM_TEXT) {
            
            UCSTextMsg * textMsg = (UCSTextMsg*)messageContent;
            _content = [textMsg.content emojizedString];
            
        }else if (message.messageType == UCS_IM_IMAGE){
            
            _content = @"图片";
            
        }else if(message.messageType == UCS_IM_VOICE){
            
            _content = @"语音";
            
        }else if(message.messageType == UCS_IM_Location){
            
            _content = @"位置";
            
        }else if(message.messageType == UCS_IM_DiscussionNotification){
            
             UCSDiscussionNotification * notificationMsg = (UCSDiscussionNotification *)message.content;
            _content = notificationMsg.extension;
            
        }else if(message.messageType == UCS_IM_Custom){
            _content = @"自定义消息";
        }else{
            
            _content = @"收到的消息是未定义的,请尽快定义";
            
        }
        
        _conversationType = message.conversationType;
        
        if (message.conversationType == UCS_IM_SOLOCHAT) {  //单聊
            
            _chatter = message.senderUserId;
            AddressBook * book = [[AddressBookManager sharedInstance] checkAddressBook:_chatter];
            _userName = (book.name)?(book.name):(message.senderUserId);
            
        }else if(message.conversationType == UCS_IM_DISCUSSIONCHAT){  //讨论组
            
            _discussionName = message.receiveId;
            AddressBook * book = [[AddressBookManager sharedInstance] checkAddressBook:message.senderUserId];
            _userName = (book.name)?(book.name):(message.senderUserId);
            
        }else if(message.conversationType == UCS_IM_GROUPCHAT){  //群组
            
            _chatter = message.receiveId;
            //从as获取群名
            NSString * groupName = [[NSUserDefaults standardUserDefaults] stringForKey:_chatter];
            _groupName = groupName?groupName:@"体验群";
            
            AddressBook * book = [[AddressBookManager sharedInstance] checkAddressBook:message.senderUserId];
            _userName = (book.name)?(book.name):(message.senderUserId);
        }
        
    }
    return self;
}

@end
