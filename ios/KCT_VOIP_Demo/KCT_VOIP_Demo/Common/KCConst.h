//
//  KCConst.h
//  KCT_VOIP_Demo_1.0
//
//  Created by KCT on 15/5/25.
//  Copyright (c) 2015年 KCT. All rights reserved.
//

#import <Foundation/Foundation.h>

#pragma mark 登陆结果通知
extern NSString * const KCLoginStateChangedNotication;
extern NSString * const KCLoginStateConnectingNotification ; //连接中....
extern NSString * const KCLoginStateLoginSuccessNotification  ; //登录成功
extern NSString * const KCLoginStateLoginFailureNotification ; //登录失败
extern NSString * const KCLoginStateNetErrorNotification   ; //网络不给力
extern NSString * const KCLoginStateLoginOutNotification  ;  //退出
extern NSString * const KCTCPConnectingNotification ;  // tcp正在连接
extern NSString * const KCTCPDisConnectNotification ;  // tcp断开连接
extern NSString * const KCTCPDidConnectNotification ;  // tcp已经连接
extern NSString * const KCTCPNoNetWorkNotification ;   // 没网
extern NSString * const KCTCPHaveNetWorkNotification ; // 有网
//消息未读数变化通知
extern NSString * const UnReadMessageCountChangedNotification;

//会话列表变化通知，来了新消息，自己发了消息等
extern NSString * const ConversationListDidChangedNotification ;

//收到新的聊天信息
extern NSString * const DidReciveNewMessageNotifacation;

// 收到语音消息下载成功或者失败的回调
extern NSString * const DidRecuveVoiceDownloadStateNotification;

//清空了聊天消息
extern NSString * const ChatMessageDidCleanNotification;

// 聊天背景改变了
extern NSString * const ChatViewBackImageDidChangedNotification  ;

//讨论组好友成员增加了
extern NSString * const DiscussionMembersDidAddNotification;

//主动退出了讨论组
extern NSString * const DidQuitDiscussionNotification;

//创建讨论组成功,rootViewController跳转到chatviewCotroller
extern NSString * const DidCreateDiscussionNotification;

//本地通知体中的key
extern NSString * const LocationNotificationChatterKey;

// 收到删除空的会话的通知
extern NSString * const RemoveEmptyConversationNotification;

// 讨论组名称被修改了
extern NSString * const DiscussionNameChanged;

// 自己被踢出讨论组
extern NSString * const RemovedADiscussionNotification;

// tcp连接状态
extern NSString * const TCPConnectStateNotification;

// 自己被踢线
extern NSString * const TCPKickOffNotification;
