//
//  UCSSendMessageQueue.m
//  Tcplib
//
//  Created by KCT   on 15/10/28.
//  Copyright © 2016年 KCTMAC. All rights reserved.
//

#import "UCSSendMessageQueue.h"

#import "KCTMessageFrame.h"

#define MAX_CACHE_FRAME             100 //消息发送队列最大数

@interface UCSSendMessageQueue()
{
    
    NSMutableArray * _sendQueueArray;  //消息发送队列
    NSMutableDictionary *_recvDelegateTable; // 根据流水号，转发收到的消息
}

@end


static UCSSendMessageQueue *g_instance = nil;


@implementation UCSSendMessageQueue


+(instancetype)instance
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        g_instance = [[self alloc] init];
    });
    return g_instance;
}


- (id)init
{
    if (self = [super init]) {
        
        _sendQueueArray = [[NSMutableArray alloc] init];
        _recvDelegateTable = [[NSMutableDictionary alloc] init];
        
    }
    return self;
}

- (void)dealloc{
    
    [_sendQueueArray removeAllObjects];
    _sendQueueArray = nil;
    
    [_recvDelegateTable removeAllObjects];
    _recvDelegateTable = nil;
}


/**
 *  @添加新的消息到消息队列里面
 */
- (void)addNewSendMessage:(NSData *)message andSendDelegate:(id)sendDelegate andRevivedNoti:(BOOL)noti{
    
    KCTMessageFrame * frame = [KCTMessageFrame decode:message];
    frame.delegate = sendDelegate;
    
    
    if ([_sendQueueArray count] > MAX_CACHE_FRAME) {
//        UXLogAlert(@"** 缓存达到上限 **");
        [_sendQueueArray removeAllObjects];
    }
    //保存暂时没有发送的包
    [_sendQueueArray addObject:frame];
    
    if (noti) {
        //如果需要服务器接到包后回调 则需要在这里添加
//        _recvDelegateTable setObject:sendDelegate forKey:[NSString stringWithFormat:@"%d",frame.message_head.seq];
    }
    
}

/**
 *  @从队列移除消息
 */
- (void)removeMessageWithSep:(int)seq{
    
    @synchronized(_sendQueueArray) {
        for (KCTMessageFrame *tmpf in _sendQueueArray) {
            if (tmpf.seq == seq) {
                [_sendQueueArray removeObject:tmpf];
                break;
            }
        }
    }
    
}
/**
 *  @从队列移除所有消息
 */
- (void)removeAllMessage{
    [_sendQueueArray removeAllObjects];
    [_recvDelegateTable removeAllObjects];
}

/**
 *  @消息队列里面的消息重发
 */
- (void)reSendAllMessageWithFaileError:(id)error{
    
    // 登录成功，如果有请求没完成，则继续完成
    NSArray *temp = [_sendQueueArray copy];
    [self removeAllMessage];
    for (KCTMessageFrame *frame in temp) {
        //后面添加
//        if ([frame type] == UX_TCP_SERVER) {
//            continue; // 过滤重连登陆、登出等消息
//        }
        //这里需要移除服务器反馈的通知的代理列表
        
        if (frame.delegate && [frame.delegate respondsToSelector:@selector(tcpSendFailedWithMessage:andError:)]) {
            
            [frame.delegate performSelector:@selector(tcpSendFailedWithMessage:andError:) withObject:frame withObject:error];
            continue; // 如果委托确认，由委托方负责是否重发;
        }
    }
    
}

/**
 *  @收到服务器的消息反馈后通知发送该条消息的代理
 */
- (void)onRecivedMessageWithSeq:(int)seq{
    
    //接到服务器反馈包 回馈给代理
    
//    id delegate = [_recvDelegateTable objectForKey:[NSString stringWithFormat:@"%d",seq]];
    
}


@end
