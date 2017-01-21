//
//  KCConst.m
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/5/25.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import "KCConst.h"

NSString * const KCLoginStateChangedNotication = @"KCLoginStateChangedNotication";
NSString * const KCLoginStateConnectingNotification = @"KCLoginStateConnectingNotification";  //连接中....
NSString * const KCLoginStateLoginSuccessNotification = @"KCLoginStateLoginSuccessNotification";  //登录成功
NSString * const KCLoginStateLoginFailureNotification = @"KCLoginStateLoginFailureNotification" ; //登录失败
NSString * const KCLoginStateNetErrorNotification = @"KCLoginStateNetErrorNotification" ; //网络不给力
NSString * const KCLoginStateLoginOutNotification = @"KCLoginStateLoginOutNotification" ; //退出
NSString * const KCTCPConnectingNotification = @"KCTCPConnectingNotification" ;  // tcp正在连接
NSString * const KCTCPDisConnectNotification = @"KCTCPDisConnectNotification" ;  // tcp断开连接
NSString * const KCTCPDidConnectNotification = @"KCTCPDidConnectNotification" ;  // tcp已经连接
NSString * const KCTCPNoNetWorkNotification = @"KCTCPNoNetWorkNotification"   ;  // 没网
NSString * const KCTCPHaveNetWorkNotification = @"KCTCPHaveNetWorkNotification"; // 有网

//消息未读数变化通知
NSString * const UnReadMessageCountChangedNotification = @"UnReadMessageCountChangedNotification";

//会话列表变化通知，来了新消息，自己发了消息等
NSString * const ConversationListDidChangedNotification = @"ConversationListDidChangedNotification";

//收到新的聊天信息
NSString * const DidReciveNewMessageNotifacation = @"DidReciveNewMessageNotifacation";
// 收到语音消息下载成功或者失败的回调
NSString * const DidRecuveVoiceDownloadStateNotification = @"DidRecuveVoiceDownloadStateNotification";

//清空了聊天消息
NSString * const ChatMessageDidCleanNotification = @"ChatMessageDidCleanNotification";

// 聊天背景改变了
NSString * const ChatViewBackImageDidChangedNotification = @"chatViewBackImageDidChangedNotification" ;

//讨论组好友成员增加了
NSString * const DiscussionMembersDidAddNotification = @"discussionMembersDidAddNotification";

//主动退出讨论组
NSString * const DidQuitDiscussionNotification = @"didQuitDiscussionNotification";

//创建讨论组成功,rootViewController跳转到chatviewCotroller
NSString * const DidCreateDiscussionNotification = @"didCreateDiscussionNotification";

//本地通知体中的key
NSString * const LocationNotificationChatterKey = @"LocationNotificationChatterKey";

// 收到删除空的会话的通知
NSString * const RemoveEmptyConversationNotification =                                       @"RemoveEmptyConversationNotification";

// 讨论组名称被修改了
NSString * const DiscussionNameChanged = @"DiscussionNameChanged";

// 自己被踢出讨论组
NSString * const RemovedADiscussionNotification = @"RemovedADiscussionNotification";

// tcp连接状态
NSString * const TCPConnectStateNotification = @"TCPConnectStateNotification";

// 自己被踢线
NSString * const TCPKickOffNotification = @"TCPKickOffNotification";
