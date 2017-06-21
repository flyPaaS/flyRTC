//
//  UCSSendMessageQueue.h
//  Tcplib
//
//  Created by KCT   on 15/10/28.
//  Copyright © 2016年 KCTMAC. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface UCSSendMessageQueue : NSObject

/**
 *  @单例对象获取方法
 */
+ (instancetype)instance;


/**
 *  @添加新的消息到消息队列里面
 */
- (void)addNewSendMessage:(NSData *)message andSendDelegate:(id)sendDelegate andRevivedNoti:(BOOL)noti;

/**
 *  @从队列移除消息
 */
- (void)removeMessageWithSep:(int)seq;

/**
 *  @从队列移除所有消息
 */
- (void)removeAllMessage;

/**
 *  @消息队列里面的消息重发
 */
- (void)reSendAllMessageWithFaileError:(id)error;


/**
 *  @收到服务器的消息反馈后通知发送该条消息的代理
 */
- (void)onRecivedMessageWithSeq:(int)seq;

@end
